# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(nl-json)

add_library(ac-dict INTERFACE)
add_library(ac::dict ALIAS ac-dict)
target_include_directories(ac-dict INTERFACE .)
target_link_libraries(ac-dict INTERFACE
    nlohmann_json::nlohmann_json
    ac::astl
)
