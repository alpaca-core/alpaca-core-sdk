# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

add_executable(DummyExample DummyExample.swift)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(DummyExample PROPERTIES COMPILE_OPTIONS "")

# Since wrapper library has it, the executable must has enabled it too
set(CMAKE_Swift_FLAGS "-cxx-interoperability-mode=default")
set(CMAKE_Swift_FLAGS "${CMAKE_Swift_FLAGS} -import-objc-header ${CMAKE_SOURCE_DIR}/wrapper/swift/util/DummyInference-Bridging-Header.h")

target_link_libraries(DummyExample
    PRIVATE
        CAlpacaCore
        ac-local-dummy
        DummyInference
)

target_compile_options(DummyExample PRIVATE -parse-as-library)
