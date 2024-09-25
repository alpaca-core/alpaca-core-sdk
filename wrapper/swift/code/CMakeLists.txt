# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
project (AlpacaCoreSwiftWrapper  LANGUAGES Swift CXX)

add_library(AlpacaCoreObjC STATIC DummyACapi.mm DummyACapi.h)

target_link_libraries(AlpacaCoreObjC
    PUBLIC
        ac::api
        splat::splat
        ac::astl
        nlohmann_json::nlohmann_json
)

add_library(AlpacaCoreSW STATIC DummyACapi.swift)
# Add C++ interop capability to the Swift target
set(CMAKE_Swift_FLAGS "-cxx-interoperability-mode=default")
# check if we're going to
#set(CMAKE_Swift_FLAGS "${CMAKE_Swift_FLAGS} -import-objc-header ${CMAKE_SOURCE_DIR}/iOS/apps/hello-ios-Bridging-Header.h" )
# Link the Payload Obj-C++ wrapper library
target_link_libraries(AlpacaCoreSW PRIVATE PayloadObjCppWrapper)
target_link_libraries(AlpacaCoreSW
    PUBLIC
        splat::splat
        ac::astl
        nlohmann_json::nlohmann_json
)
