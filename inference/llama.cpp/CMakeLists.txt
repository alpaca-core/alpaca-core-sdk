# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(LLAMA_BUILD_COMMON OFF)
add_subdirectory(llama.cpp)
add_subdirectory(code)
ac_add_local_subdir()
ac_add_test_subdir()
ac_add_example_subdir()
