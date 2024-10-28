# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

include(./cmake/AddSwift.cmake)

add_library(AlpacaCoreSwift STATIC
    Expected.hpp
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
        ac-dev::audio # audio is temporary for easier whisper integration
    PRIVATE
        ac-local-dummy
        ac-local-whisper
        ac-local-llama
        jalog::jalog
        jalog::sinklib
)

_swift_generate_cxx_header(AlpacaCoreSwift
    AlpacaCore-Swift.h
    SWIFT_EXPOSABLE_FILES ModelDesc.swift
)
