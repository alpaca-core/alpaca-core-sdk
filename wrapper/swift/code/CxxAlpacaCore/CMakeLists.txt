# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(CxxAlpacaCore STATIC
    ACDict.hpp
    ACDict.cpp
)

target_include_directories(CxxAlpacaCore INTERFACE .)

target_link_libraries(CxxAlpacaCore
    PUBLIC
        ac::dict
        ac::local
)
