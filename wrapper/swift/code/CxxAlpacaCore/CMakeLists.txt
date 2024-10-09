# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

# Set up swiftrt.o and runtime library search paths
include(InitializeSwift)
# cmake/modules/AddSwift.cmake provides the function for creating the Swift to
# C++ bridging header
include(AddSwift)

add_library(AlpacaCore STATIC
    ACDict.hpp
    ACDict.cpp
    Model.hpp
    Model.cpp
    Instance.hpp
    Instance.cpp
    AlpacaCore.hpp
    AlpacaCore.cpp
    SwiftAlpacaCore.swift
    DictHelper.swift
)

set_target_properties(AlpacaCore PROPERTIES COMPILE_OPTIONS "")

target_include_directories(AlpacaCore INTERFACE .)

target_link_libraries(AlpacaCore
    PUBLIC
        ac::dict
        ac::local
        ac-local-dummy
        ac-local-whisper
        ac-local-llama
)

# Generate a C++ header from Swift sources. This is automatically added to the
# fibonacci target. The target will regenerate the header file when any of the
# Swift sources change. Clang detects that the C++ file depends on the header,
# and tells Ninja about this dependency in the depfile.
# This function is implemented in cmake/modules/AddSwift.cmake.
_swift_generate_cxx_header(AlpacaCore
  ${CMAKE_CURRENT_SOURCE_DIR}/AlpacaCore-Swift.h
  SEARCH_PATHS "${CMAKE_CURRENT_SOURCE_DIR}"
)
