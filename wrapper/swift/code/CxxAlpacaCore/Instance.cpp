// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Instance.hpp"
#include "AlpacaCore.hpp"
#include "Dict.hpp"

#include <ac/local/Instance.hpp>

#include <swift/bridging>
#include "AlpacaCore-Swift.h"

namespace AC {

Instance::Instance(std::unique_ptr<local::Instance> instance)
    : m_instance(std::move(instance))
{}

Instance::Instance(const Instance& other) {
    m_instance = other.m_instance;
}

Expected<DictRoot, std::string> Instance::runOp(const std::string& op, DictRef params, ProgressCallbackData progressCbData) {
    DictRoot root;
    DictRef ref = root.getRef();

    try {
        ref.getDict() = m_instance->runOp(op, params.getDict(), [&](std::string_view tag, float progress) {
            progressCbData.m_cb(progressCbData.m_context, tag.data(), progress);
            return true;
        });
        return root;
    } catch(const std::exception& e) {
        return itlib::unexpected<std::string>(e.what());
    }
}

}
