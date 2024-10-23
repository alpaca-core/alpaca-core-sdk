// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"

#include "Instance.hpp"

namespace AC {

Model::Model(local::ModelPtr model)
    : m_model(model)
{}

Expected<Instance, std::string> Model::createInstance(const std::string& type, DictRef params)  {
    try {
        return Instance(m_model->createInstance(type, params.m_dictRef));
    }
    catch(const std::exception& e) {
        return itlib::unexpected<std::string>(e.what());
    }
}

}
