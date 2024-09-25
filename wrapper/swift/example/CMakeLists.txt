# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(DummyExample DummyExample.swift)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(DummyExample PROPERTIES COMPILE_OPTIONS "")

# Add C++ interop capability to the Swift target
set(CMAKE_Swift_FLAGS "-cxx-interoperability-mode=default")
set(CMAKE_Swift_FLAGS "${CMAKE_Swift_FLAGS} -import-objc-header ${CMAKE_SOURCE_DIR}/wrapper/swift/code/DummyACapi-Bridging-Header.h" )

# Include the directory where the Obj-C++ wrapper is located
target_include_directories(DummyExample PRIVATE
    ${CMAKE_SOURCE_DIR}/wrapper/swift/code
)

target_link_libraries(DummyExample PUBLIC AlpacaCoreSW)
target_link_libraries(DummyExample PUBLIC AlpacaCoreObjC)

target_compile_options(DummyExample PRIVATE -parse-as-library)
