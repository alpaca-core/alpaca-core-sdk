# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-local-llama AC_LOCAL_LLAMA)
add_library(ac::local-llama ALIAS ac-local-llama)

target_include_directories(ac-local-llama PUBLIC code)

target_sources(ac-local-llama PRIVATE
    code/ac/export.h
    code/ac/LocalLlama.hpp
    code/ac/LocalLlama.cpp
    code/ac/local_llama.h
    code/ac/local_llama.cpp
    code/ac/LocalLlamaSchema.hpp
)

target_link_libraries(ac-local-llama
    PRIVATE
        ac::llama
        ac::local
        ac::model-schema
)

ac_add_example_subdir()
