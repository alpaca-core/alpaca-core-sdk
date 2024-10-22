# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
find_package(XCTest QUIET)
if(NOT XCTest_FOUND)
    message(STATUS "Could not find XCTest! Skpping Swift wrapper tests.")
    return()
endif()

include(FindSwiftXCTest)

add_swift_xctest(acSwiftTests AlpacaCoreSwift t-Dict.swift t-SwiftApi.swift)
set_target_properties(acSwiftTests PROPERTIES COMPILE_OPTIONS "")
