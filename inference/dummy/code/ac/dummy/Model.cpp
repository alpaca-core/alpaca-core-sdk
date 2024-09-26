// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "Model.hpp"
#include "Logging.hpp"
#include <astl/throw_ex.hpp>
#include <astl/move.hpp>
#include <fstream>

namespace ac::dummy {

Model::Model(const char* path, Params params)
    : m_params(astl::move(params))
{
    std::ifstream file(path);
    if (!file.is_open()) {
        DUMMY_LOG(Error, "Failed to open file: ", path);
        throw_ex{} << "Failed to open file: " << path;
    }

    DUMMY_LOG(Info, "Loading model from ", path);

    while (!file.eof()) {
        std::string word;
        file >> word;
        if (word.empty()) {
            // ignore empty strings
            continue;
        }
        addDataItem(astl::move(word));
    }
}

static constexpr std::string_view SyntheticModel_Data[] = {
    "one", "two", "three", "four", "five", "once", "I", "caught", "a", "fish", "alive",
    "six", "seven", "eight", "nine", "ten", "then", "I", "let", "it", "go", "again"
};

Model::Model(Params params)
    : m_params(astl::move(params))
{
    for (auto& item : SyntheticModel_Data) {
        addDataItem(std::string(item));
    }
}

Model::~Model() = default;

void Model::addDataItem(std::string item) {
    if (!m_params.splice.empty()) {
        m_data.push_back(m_params.splice);
    }
    m_data.push_back(astl::move(item));
}

std::span<const std::string_view> Model::rawSyntheticModelData() noexcept {
    return SyntheticModel_Data;
}

} // namespace ac::dummy
