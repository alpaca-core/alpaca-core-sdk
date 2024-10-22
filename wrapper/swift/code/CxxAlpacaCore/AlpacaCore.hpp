// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include <ac/local/ModelFactory.hpp>
#include <itlib/expected.hpp>

#include "AlpacaCore-Swift.h"

namespace AC {

typedef void (*SwiftProgressCb)(void* _Nonnull context, const char* _Nonnull tag, float progress);

struct ProgressCallbackData {
    SwiftProgressCb _Nonnull m_cb;
    void* _Nonnull m_context;
};

template <typename T, typename E>
class Expected : public itlib::expected<T, E> {
    using super = itlib::expected<T, E>;
public:
    using super::expected;
    T value() const noexcept {
        return super::value();
    }

    E error() const noexcept {
        return super::error();
    }

    bool hasValue() const {
        return super::has_value();
    }

    bool hasError() const {
        return super::has_error();
    }
};

void initSDK();

Expected<class Model, std::string> createModel(AlpacaCoreSwift::ModelDesc& desc, class DictRef params, ProgressCallbackData progressCbData);

std::vector<float> loadWavF32Mono(const std::string& path);

}
