#pragma once

#include "../../../common/ac-audio/code/ac-audio.hpp"

namespace AC {

std::vector<float> loadWavF32Mono(const std::string& path) {
    return ac::audio::loadWavF32Mono(path);
}

}
