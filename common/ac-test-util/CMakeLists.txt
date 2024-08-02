# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-test-util INTERFACE)
target_include_directories(ac-test-util INTERFACE .)
add_library(ac-dev::test-util ALIAS ac-test-util)
