# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

# Set up swiftrt.o and runtime library search paths
include(InitializeSwift)
# cmake/modules/AddSwift.cmake provides the function for creating the Swift to
# C++ bridging header
include(AddSwift)

add_library(AlpacaCoreSwift STATIC
    Dict.hpp
    Dict.cpp
    Model.hpp
    Model.cpp
    Instance.hpp
    Instance.cpp
    AlpacaCore.hpp
    AlpacaCore.cpp
    SwiftApi.swift
    DictHelper.swift
    ModelDesc.swift
)

# If we don't clear compiler options -Wall is passed which is not recognized by Swift compiler
set_target_properties(AlpacaCoreSwift PROPERTIES COMPILE_OPTIONS "")

target_compile_options(AlpacaCoreSwift
  PUBLIC
    $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
)

target_include_directories(AlpacaCoreSwift PUBLIC .)

target_link_libraries(AlpacaCoreSwift
    PUBLIC
        ac::dict
        ac::local
        ac-dev::audio
        ac-local-dummy
        ac-local-whisper
        ac-local-llama
        ac-test-data::whisper
)

# Generate a C++ header from selected Swift sources. This is automatically added to the
# AlpacaCore target. The target will regenerate the header file when any of the
# Swift sources change. Clang detects that the C++ file depends on the header,
# and tells Ninja about this dependency in the depfile.
# This function is implemented in cmake_modules/AddSwift.cmake.
_swift_generate_cxx_header(AlpacaCoreSwift
  ${CMAKE_CURRENT_SOURCE_DIR}/AlpacaCore-Swift.h
  SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}
  SWIFT_EXPOSABLE_FILES
    ModelDesc.swift
)
