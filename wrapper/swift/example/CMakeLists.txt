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
# End of DummyInference library

add_executable(DummyExample DummyExample.swift)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(DummyExample PROPERTIES COMPILE_OPTIONS "")

# Since wrapper library has it, the executable must has enabled it too
set(CMAKE_Swift_FLAGS "-cxx-interoperability-mode=default")
set(CMAKE_Swift_FLAGS "${CMAKE_Swift_FLAGS} -import-objc-header ${CMAKE_SOURCE_DIR}/wrapper/swift/example/DummyInference-Bridging-Header.h")

target_link_libraries(DummyExample
    PRIVATE
        CAlpacaCore
        ac-local-dummy
        DummyInference
)

target_compile_options(DummyExample PRIVATE -parse-as-library)

# TODO: In order to set generate correctly the project we need to set the
# C++ Language Dialect to C++17 or above
# target_compile_options(DummyExample PRIVATE -std=c++17) # This is not working since it's setting compiler options for Swift which is a missing option
