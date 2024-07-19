// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Job.hpp"
#include "Model.hpp"
#include "Logging.hpp"
#include "ChatFormat.hpp"
#include "Vocab.hpp"
#include "Sampler.hpp"
#include "iile.h"
#include "throw_ex.hpp"
#include <llama.h>
#include <cassert>


namespace ac::llama {

namespace {
llama_context_params llamaFromJobInitParams(const Job::InitParams& params) {
    llama_context_params llamaParams = llama_context_default_params();
    return llamaParams;
}
} // namespace

Job::Job(Model& model, InitParams params)
    : m_model(model)
    , m_lctx(llama_new_context_with_model(model.lmodel(), llamaFromJobInitParams(params)), llama_free)
{
    if (!m_lctx) {
        throw_ex{} << "Failed to create llama context";
    }
    assert(model.lmodel() == llama_get_model(m_lctx.get()));

    const auto ctxLen = llama_n_ctx(m_lctx.get());
    const auto ctxTrain = model.trainCtxLength();
    if (ctxLen > ctxTrain) {
        LLAMA_LOG(Warning, "Job requested context length ", ctxLen, " is greater than the model's training context length ", ctxTrain);
    }
}

Job::~Job() = default;

void Job::warmup() {
    LLAMA_LOG(Info, "Running warmup");

    auto lctx = m_lctx.get();
    auto model = m_model.lmodel();

    std::vector<llama_token> tmp;
    llama_token bos = llama_token_bos(model);
    llama_token eos = llama_token_eos(model);
    // some models (e.g. T5) don't have a BOS token
    if (bos != -1) {
        tmp.push_back(bos);
    }
    tmp.push_back(eos);

    const auto ntokens = int32_t(tmp.size());

    if (llama_model_has_encoder(model)) {
        llama_encode(lctx, llama_batch_get_one(tmp.data(), ntokens, 0, 0));
        llama_token decoder_start_token_id = llama_model_decoder_start_token(model);
        if (decoder_start_token_id == -1) {
            decoder_start_token_id = bos;
        }
        tmp.clear();
        tmp.push_back(decoder_start_token_id);
    }
    llama_decode(lctx, llama_batch_get_one(tmp.data(), ntokens, 0, 0));
    llama_kv_cache_clear(lctx);
    llama_synchronize(lctx);
    llama_reset_timings(lctx);
}

itlib::generator<Token> Job::run(RunParams params) {
    Vocab vocab(m_model);

    Sampler sampler({});

    auto lctx = m_lctx.get();

    ChatFormat fmt(m_model.getChatTemplateId());
    std::vector<ChatMsg> chat;
    auto chatAddAndFormat = [&](std::string role, std::string text) {
        ChatMsg newMsg = {std::move(role), std::move(text)};
        auto ret = fmt.formatMsg(newMsg, chat, newMsg.role == "user");
        chat.push_back(std::move(newMsg));
        return ret;
    };

    std::vector<llama_token> inputTokens;
    if (params.prompt.empty()) {
        // Should not run without any tokens
        inputTokens.push_back(llama_token_bos(m_model.lmodel()));
    }
    else {
        if (params.conversation) {
            auto fmtChat = chatAddAndFormat("system", params.prompt);
            inputTokens = vocab.tokenize(fmtChat, true, true);
        }
        else {
            inputTokens = vocab.tokenize(params.prompt, true, true);
        }
    }

    const auto ctxLen = llama_n_ctx(m_lctx.get());
    if (inputTokens.size() > ctxLen - 4) {
        throw_ex{} << "Input too long. Got " << inputTokens.size() << " tokens, max: " << ctxLen - 4;
    }

    const auto numKeep = int32_t(inputTokens.size());

    // fix invariants
    if (params.conversation) {
        params.interactiveFirst = true;
    }
    if (params.interactiveFirst) {
        params.interactive = true;
    }

    // group-attention state
    // number of grouped KV tokens so far (used only if params.grp_attn_n > 1)
    uint32_t gaIndex = 0;

    const uint32_t gaFactor = params.gaFactor;
    const uint32_t gaWidth = params.gaWidth;

    if (gaFactor != 1) {
        if (gaWidth % gaFactor != 0) {
            throw_ex{} << "Group-attention width " << gaWidth << " must be a multiple of group-attention factor " << gaFactor;
        }
        LLAMA_LOG(Info, "self-extend: train = ", m_model.trainCtxLength(), ", gaFactor = ", gaFactor,  ", gaWidth = ", gaWidth);
    }

    bool interacting = params.interactive && params.interactiveFirst;

    uint32_t numPast = 0;
    uint32_t numRemaining = uint32_t(params.numTokensToPredict);
    uint32_t numConsumed = 0;

    std::vector<Token> tokens;

    if (m_model.hasEncoder()) {
        auto batch = llama_batch_get_one(inputTokens.data(), int32_t(inputTokens.size()), 0, 0);
        auto res = llama_encode(lctx, batch);
        if (res != 0) {
            throw_ex{} << "Failed to encode input";
        }
        inputTokens.clear();
        inputTokens.push_back(vocab.decoderStartToken());
    }

    const auto batchSize = params.batchSize;

    while (true) {
        if (!tokens.empty()) {
            // predict

            const auto maxTokens = ctxLen - 4; // matching limit from above

            // Ensure the input doesn't exceed the context size by truncating embd if necessary.
            if (tokens.size() > maxTokens) {
                const auto skipped = tokens.size() - maxTokens;
                tokens.resize(maxTokens);
                LLAMA_LOG(Warning, "Input too long. Skipping ", skipped, " tokens");
            }

            bool haveFullContextMitigation = false;
            if (gaFactor == 1) {
                // infinite text generation via context shifting
                // if we run out of context:
                // - take the n_keep first tokens from the original prompt (via numPast)
                // - take half of the last (n_ctx - n_keep) tokens and recompute the logits in batches
                const auto num = numPast + tokens.size();
                if (num >= ctxLen) {
                    if (params.numTokensToPredict == -2) {
                        LLAMA_LOG(Warning, "Context is full and infinite context was rejected => stopping");
                        break;
                    }

                    const auto numLeft = numPast - numKeep;
                    const int numDiscard = numLeft / 2; // somewhat arbitrary

                    LLAMA_LOG(Debug, "Context is full. Swapping: past = ", numPast, ", numLeft: ", numLeft,
                        ", ctxLen: ", ctxLen, ", numKeep: ", numKeep, ", numDiscard: ", numDiscard);

                    llama_kv_cache_seq_rm(lctx, 0, numKeep, numKeep + numDiscard);
                    llama_kv_cache_seq_add(lctx, 0, numKeep + numDiscard, numPast, -numDiscard);

                    numPast -= numDiscard;
                    haveFullContextMitigation = true;
                }
            }
            else {
                while (numPast >= gaIndex + gaWidth) {
                    // context extension via Self-Extend
                    const int ib = (gaFactor * gaIndex) / gaWidth;
                    const int bd = (gaWidth / gaFactor) * (gaFactor - 1);
                    const int dd = (gaWidth / gaFactor) - ib * bd - gaWidth;

                    LLAMA_LOG(Debug, "Group attention shift: ib = ", ib, ", bd = ", bd, ", dd = ", dd);

                    llama_kv_cache_seq_add(lctx, 0, gaIndex, numPast, ib * bd);
                    llama_kv_cache_seq_div(lctx, 0, gaIndex + ib * bd, gaIndex + ib * bd + gaWidth, gaFactor);
                    llama_kv_cache_seq_add(lctx, 0, gaIndex + ib * bd + gaWidth, numPast + ib * bd, dd);

                    numPast -= bd;

                    gaIndex += gaWidth / gaFactor;
                    haveFullContextMitigation = true;
                }
            }

            if (haveFullContextMitigation) {
                LLAMA_LOG(Info, "Context full mitigation performed: past = ", numPast, ", tokens = ", tokens.size());
            }

            for (uint32_t i = 0; i < tokens.size(); i += batchSize) {
                const auto numEval = std::min(batchSize, uint32_t(tokens.size() - i));

                auto batch = llama_batch_get_one(tokens.data() + i, numEval, numPast, 0);
                if (llama_decode(lctx, batch) != 0) {
                    throw_ex{} << "Failed to decode tokens";
                }

                numPast += numEval;
            }
        }

        tokens.clear();

        bool produced = false; // has the model produced any output or is this just the input echo

        if (inputTokens.size() <= numConsumed && !interacting) {
            const auto token = sampler.sample(lctx);
            sampler.accept(token);

            tokens.push_back(token);

            --numRemaining; // dec remaining sampling budget
            produced = true;
        }
        else {
            // some user input remains from prompt or interaction, forward it to processing
            while (inputTokens.size() > numConsumed) {
                tokens.push_back(inputTokens[numConsumed]);

                // push the prompt in the sampling context in order to apply repetition penalties later
                // for the prompt, we don't apply grammar rules
                sampler.accept(inputTokens[numConsumed]);

                ++numConsumed;
                if (tokens.size() >= batchSize) {
                    break;
                }
            }
        }

        if (produced) {
            for (auto id : tokens) {
                co_yield id;
            }
        }

        // if not currently processing queued inputs;
        if (inputTokens.size() <= numConsumed) {
            // deal with end of generation tokens in interactive mode
            if (vocab.isEog(sampler.last()) && params.interactive) {
                if (params.conversation) {
                    // so here we add just some random message to the conversation to keep the flow of the conversation
                    // it is not going to be used otherwise
                    chatAddAndFormat("assistant", "msg");
                }
                interacting = true;
            }

            if (numPast > 0) {
                if (interacting) {
                    // wait for user input
                    sampler.reset();
                }
                interacting = false;
            }
        }

        if (!tokens.empty() && vocab.isEog(tokens.back()) && !params.interactive) {
            co_return;
        }

        // In interactive mode, respect the maximum number of tokens and drop back to user input when reached.
        // We skip this logic when n_predict == -1 (infinite) or -2 (stop at context size).
        if (params.interactive && numRemaining <= 0 && params.numTokensToPredict >= 0) {
            numRemaining = params.numTokensToPredict;
            interacting = true;
        }

        if (numRemaining == 0) {
            co_return;
        }
    }
}

} // namespace ac::llama
