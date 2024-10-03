# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

add_library(DummyInference STATIC DummyInference.h DummyInference.mm)

target_link_libraries(DummyInference
    PUBLIC
        ac::local
        ac::local-dummy
        splat::splat
)

target_include_directories(DummyInference
    PUBLIC
        ${CMAKE_SOURCE_DIR}/inference/dummy/local/code
)

# TODO: In order to set generate correctly the project we need to set the
# C++ Language Dialect to C++17 or above
set_target_properties(DummyInference PROPERTIES
    XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++20"
)
