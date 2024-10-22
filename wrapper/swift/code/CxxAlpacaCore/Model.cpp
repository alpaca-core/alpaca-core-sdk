// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"

#include "Instance.hpp"

namespace AC {

Model::Model(local::ModelPtr model)
    : m_model(model)
{}

Instance Model::createInstance(const std::string& type, DictRef params)  {
    return Instance(m_model->createInstance(type, *params.m_dictRef));
}

}
