# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-local-llama AC_LOCAL_LLAMA)
add_library(ac::local-llama ALIAS ac-local-llama)

target_include_directories(ac-local-llama PUBLIC code)

target_sources(ac-local-llama PRIVATE
    code/ac/local/export.h
    code/ac/local/LocalLlama.hpp
    code/ac/local/LocalLlama.cpp
)

target_link_libraries(ac-local-llama
    PRIVATE
        ac::llama
        ac::local
)

ac_add_tools_subdir()
ac_add_example_subdir()
