// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <string>
#include <vector>
#include <span>
#include <string_view>

namespace ac::dummy {

class AC_DUMMY_EXPORT Model {
public:
    struct Params {
        std::string splice; // splice string before each data element
    };

    Model(const char* path, Params params);
    explicit Model(Params params); // synthetic model
    ~Model();

    const std::vector<std::string>& data() const noexcept { return m_data; }

    static std::span<const std::string_view> rawSyntheticModelData() noexcept;
private:
    void addDataItem(std::string item);

    const Params m_params;
    std::vector<std::string> m_data;
};

} // namespace ac::dummy
