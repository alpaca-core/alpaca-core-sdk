// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"
#include "Logging.hpp"
#include <astl/throw_ex.hpp>
#include <fstream>

namespace ac::dummy {

Model::Model(const char* path, Params params)
    : m_params(std::move(params))
{
    std::ifstream file(path);
    if (!file.is_open()) {
        DUMMY_LOG(Error, "Failed to open file: ", path);
        throw_ex{} << "Failed to open file: " << path;
    }

    while (!file.eof()) {
        std::string word;
        file >> word;
        if (word.empty()) {
            // ignore empty strings
            continue;
        }
        if (!m_params.splice.empty()) {
            m_data.push_back(m_params.splice);
        }
        m_data.push_back(std::move(word));
    }
}

Model::~Model() = default;

} // namespace ac::dummy
