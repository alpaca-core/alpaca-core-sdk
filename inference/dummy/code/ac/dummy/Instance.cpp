// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Instance.hpp"
#include "Model.hpp"
#include <astl/throw_ex.hpp>

namespace ac::dummy {

Instance::Instance(Model& model, InitParams params)
    : m_params(std::move(params))
    , m_model(model)
    , m_data(model.data())
{
    if (params.cutoff >= 0) {
        if (params.cutoff > int(m_data.size())) {
            throw_ex{} << "Cutoff " << params.cutoff << " greater than model size " << m_data.size();
        }
        m_data = m_data.subspan(0, size_t(params.cutoff));
    }
}

Instance::~Instance() = default;

itlib::generator<const std::string&> Instance::newSession(std::vector<std::string> input, SessionParams params) {
    int i = 0;
    auto maybeThrow = [&]() {
        if (i == params.throwOn) {
            throw_ex{} << "Throw on token " << i;
        }
        ++i;
    };

    if (params.splice) {
        if (m_data.empty()) co_return;

        size_t di = 0;
        for (auto& w : input) {
            maybeThrow();
            co_yield w;
            maybeThrow();
            co_yield m_data[di++];
            if (di == m_data.size()) {
                di = 0;
            }
        }
    }
    else {
        for (auto& word : input) {
            maybeThrow();
            co_yield word;
        }
        for (auto& word : m_data) {
            maybeThrow();
            co_yield word;
        }
    }
}

} // namespace ac::dummy
