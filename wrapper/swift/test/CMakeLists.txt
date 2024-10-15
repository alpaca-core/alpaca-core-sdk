# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
include(FindSwiftXCTest)

add_swift_xctest(acSwiftTests AlpacaCoreSwift t-Dict.swift t-SwiftApi.swift)
set_target_properties(acSwiftTests PROPERTIES COMPILE_OPTIONS "")
