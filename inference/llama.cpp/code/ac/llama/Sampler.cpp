// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Sampler.hpp"
#include <llama.h>

namespace ac::llama {

Sampler::Sampler(Params params) : m_params(params) {

}
Sampler::~Sampler() = default;

} // namespace ac::llama
