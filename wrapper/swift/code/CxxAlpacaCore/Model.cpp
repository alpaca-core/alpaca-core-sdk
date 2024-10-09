// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"

#include "Instance.hpp"

namespace ac {

Model::Model(local::ModelPtr model)
    : m_model(model)
{}

Model* Model::create(local::ModelPtr model) {
    return new Model(model);
}

Instance* Model::createInstance(const std::string& type, DictRef params)  {
        return new Instance(m_model->createInstance(type, *params.m_dictRef));
    }
}

void retainModel(ac::Model* _Nullable d) {
    d->retain();
}

void releaseModel(ac::Model* _Nullable d) {
    d->release();
}
