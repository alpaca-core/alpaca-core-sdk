# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(astl INTERFACE)
add_library(ac::astl ALIAS astl)
target_include_directories(astl INTERFACE include)
target_link_libraries(astl INTERFACE itlib::itlib splat::splat)

ac_add_test_subdir()
