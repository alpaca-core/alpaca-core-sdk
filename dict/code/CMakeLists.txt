# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
# set(NLOHMANN_JSON_TARGET_NAME ac-json)
add_library(ac-dict INTERFACE)
add_library(ac::dict ALIAS ac-dict)
target_include_directories(ac-dict INTERFACE .)
