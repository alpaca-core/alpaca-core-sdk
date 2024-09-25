# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
project (AlpacaCoreSwiftWrapper
    LANGUAGES Swift CXX
    )

add_library(AlpacaCoreObjC STATIC DummyACapi.mm DummyACapi.h)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(AlpacaCoreObjC PROPERTIES COMPILE_OPTIONS "")

add_compile_options(-fvisibility=default)

target_link_libraries(AlpacaCoreObjC
    PUBLIC
        ac::api
)

target_include_directories(AlpacaCoreObjC
    PUBLIC
        ${CMAKE_SOURCE_DIR}/api/code
)

add_library(AlpacaCoreSW STATIC DummyACapi.swift)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(AlpacaCoreSW PROPERTIES COMPILE_OPTIONS "")

# Add C++ interop capability to the Swift target
# check if we're going to use the bridging header
set(CMAKE_Swift_FLAGS "-cxx-interoperability-mode=default")
set(CMAKE_Swift_FLAGS "${CMAKE_Swift_FLAGS} -import-objc-header ${CMAKE_SOURCE_DIR}/wrapper/swift/code/DummyACapi-Bridging-Header.h" )

# Link the Payload Obj-C++ wrapper library
target_link_libraries(AlpacaCoreSW PUBLIC AlpacaCoreObjC)
