# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-llama AC_LLAMA)
add_library(ac::llama ALIAS ac-llama)
target_include_directories(ac-llama INTERFACE .)
target_link_libraries(ac-llama
    PRIVATE
        llama
        jalog::jalog
    PUBLIC
        ac::api
)
target_sources(ac-llama PRIVATE
    ac/llama/export.h
    ac/llama/Init.hpp
    ac/llama/Init.cpp
    ac/llama/Logging.hpp
    ac/llama/Logging.cpp
    ac/llama/Model.hpp
    ac/llama/Model.cpp
    ac/llama/Job.hpp
    ac/llama/Job.cpp
)
