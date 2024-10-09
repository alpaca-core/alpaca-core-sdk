// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"
#include <iostream>

namespace ac {

void printModelDesc(AlpacaCore::ModelDesc& desc) {
    std::cout << "ModelDesc:" << std::endl;
    std::cout << "  InferenceType: " << (std::string)desc.getM_inferenceType() << std::endl;
    std::cout << "  Name: " << (std::string)desc.getM_name() << std::endl;
    std::cout << "  Assets:" << std::endl;
    for (auto asset : desc.getM_assets()) {
        std::cout << "    Path: " << (std::string)asset.getM_path() << std::endl;
        std::cout << "    Tag: " << (std::string)asset.getM_tag() << std::endl;
    }

    std::cout << "kur" << std::endl;
}

}
