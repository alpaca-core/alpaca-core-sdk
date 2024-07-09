# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-core INTERFACE)
add_library(ac::core ALIAS ac-core)
target_include_directories(ac-core INTERFACE .)
target_link_libraries(ac-core INTERFACE
    nlohmann_json::nlohmann_json
)
