// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include "AlpacaCore.hpp"
#include "Dict.hpp"

#include <ac/local/Instance.hpp>

namespace ac::swift {

class Instance {
public:
    Instance(const Instance& other);
    DictRoot runOp(const std::string& op, DictRef params, ProgressCallbackData progressCbData);
private:
    friend class Model;
    Instance(std::unique_ptr<local::Instance> instance);
    std::shared_ptr<local::Instance> m_instance;
};

}
