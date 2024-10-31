// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/ModelSchema.hpp>

namespace ac::local::schema {

struct Llama : public ModelHelper<Llama> {
    static inline constexpr std::string_view id = "llama.cpp";
    static inline constexpr std::string_view description = "Inference based on our fork of https://github.com/ggerganov/llama.cpp";

    using Params = Null;

    struct InstanceGeneral : public InstanceHelper<InstanceGeneral> {
        static inline constexpr std::string_view id = "general";
        static inline constexpr std::string_view description = "General instance";

        // using Params = Null;
        struct Params : public Object {
            using Object::Object;
            Uint ctxSize{*this, "ctx_size", "Size of the contex", {}};
            Uint batchSize{*this, "batch_size", "Size of the single batch", {}};
            Uint ubatchSize{*this, "ubatch_size", "Size of the contex", {}};
        };

        struct OpRun {
            static inline constexpr std::string_view id = "run";
            static inline constexpr std::string_view description = "Run the llama.cpp inference and produce some output";

            struct Params : public Object {
                using Object::Object;
                String prompt{*this, "prompt", "Prompt to complete", ""};
                Array<String> antiprompts{*this, "antiprompts", "Antiprompts to trigger stop", Dict::array()};
                Uint maxTokens{*this, "max_tokens", "Maximum number of tokens to generate. 0 for unlimited", 0};
            };

            struct Return : public Object {
                using Object::Object;
                String result{*this, "result", "Generated result (completion of prompt)", {}, true};
            };
        };

        struct OpChatBegin {
            static inline constexpr std::string_view id = "begin-chat";
            static inline constexpr std::string_view description = "Begin a chat session";

            struct Params : public Object {
                using Object::Object;
                String setup{*this, "setup", "Initial setup for the chat session", ""};
                String roleUser{*this, "role_user", "Role name for the user", "User"};
                String roleAssistant{*this, "role_assistant", "Role name for the assistant", "Assistant"};
            };

            using Return = Null;
        };

        struct OpChatAddPrompt {
            static inline constexpr std::string_view id = "add-chat-prompt";
            static inline constexpr std::string_view description = "Add a prompt to the chat session as a user";

            struct Params : public Object {
                using Object::Object;
                String prompt{*this, "prompt", "Prompt to add to the chat session", ""};
            };

            using Return = Null;
        };

        struct OpChatGetResponse {
            static inline constexpr std::string_view id = "get-chat-response";
            static inline constexpr std::string_view description = "Get a response from the chat session";

            using Params = Null;

            struct Return : public Object {
                using Object::Object;
                String response{*this, "response", "Response from the chat session", {}, true};
            };
        };

        using Ops = std::tuple<OpRun, OpChatBegin, OpChatAddPrompt, OpChatGetResponse>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

}  // namespace ac::local::schema
