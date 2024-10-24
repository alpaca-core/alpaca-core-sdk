// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include "../../../common/ac-audio/code/ac-audio.hpp"

namespace AC {

inline std::vector<float> loadWavF32Mono(const std::string& path) {
    return ac::audio::loadWavF32Mono(path);
}

}
