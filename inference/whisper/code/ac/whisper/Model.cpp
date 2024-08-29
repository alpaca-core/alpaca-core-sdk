// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"
#include <whisper.h>
#include <astl/move.hpp>
#include <stdexcept>

namespace ac::whisper {

Model::Model(const char* pathToBin, Params params)
    : m_params(astl::move(params))
    , m_pathToBin(pathToBin)
{}

Model::~Model() = default;


} // namespace ac::whisper
