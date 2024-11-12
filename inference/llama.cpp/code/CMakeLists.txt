# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-llama AC_LLAMA)
add_library(ac::llama ALIAS ac-llama)
target_include_directories(ac-llama INTERFACE .)
target_link_libraries(ac-llama
    PRIVATE
        llama
        ac::jalog
    PUBLIC
        splat::splat
)
target_sources(ac-llama PRIVATE
    ac/llama/export.h
    ac/llama/Init.hpp
    ac/llama/Init.cpp
    ac/llama/Logging.hpp
    ac/llama/Logging.cpp
    ac/llama/Model.hpp
    ac/llama/Model.cpp
    ac/llama/ChatFormat.hpp
    ac/llama/ChatFormat.cpp
    ac/llama/Vocab.hpp
    ac/llama/Vocab.cpp
    ac/llama/Sampler.hpp
    ac/llama/Sampler.cpp
    ac/llama/Instance.hpp
    ac/llama/Instance.cpp
    ac/llama/Session.hpp
    ac/llama/AntipromptManager.hpp
    ac/llama/AntipromptManager.cpp
    ac/llama/IncrementalStringFinder.hpp
    ac/llama/IncrementalStringFinder.cpp
)
