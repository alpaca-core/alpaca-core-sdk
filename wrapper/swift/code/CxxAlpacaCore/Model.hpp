// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include "AlpacaCore.hpp"
#include "Expected.hpp"
#include "Dict.hpp"

#include <ac/local/Model.hpp>
#include <ac/local/ModelPtr.hpp>

namespace AC {
class Instance;

class Model{
public:
    Model(local::ModelPtr model);

    Expected<Instance, std::string> createInstance(const std::string& type, DictRef params);
private:
    local::ModelPtr m_model;
};

}
