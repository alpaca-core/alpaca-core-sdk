# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# enable_testing()

# # Config XCTest
# find_package(XCTest REQUIRED)
# set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED "NO")
# set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED "NO")
# set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "")

# set(AC_TEST_SOURCE
#     ./TestDict.mm
# )

# xctest_add_bundle(test-ac-objc CAlpacaCore
#     ${AC_TEST_SOURCE}
# )

# set_target_properties(test-ac-objc PROPERTIES
#     MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
# )

# target_include_directories(test-ac-objc PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )
# find_package(XCTest REQUIRED)
# xctest_add_bundle(TestAlpacaCoreSwift AlpacaCoreSwift
#     t-Dict.swift
# )

include(FetchContent)
FetchContent_Declare(SwiftTesting
  GIT_REPOSITORY https://github.com/swiftlang/swift-testing.git
  GIT_TAG main)
FetchContent_MakeAvailable(SwiftTesting)

add_executable(TestAlpacaCoreSwift
    t-Dict.swift
)

set_target_properties(TestAlpacaCoreSwift PROPERTIES COMPILE_OPTIONS "")
set_target_properties(TestAlpacaCoreSwift PROPERTIES
  SUFFIX .swift-testing
)
target_link_libraries(TestAlpacaCoreSwift PRIVATE
  AlpacaCoreSwift
  Testing
)

# set_target_properties(TestAlpacaCoreSwift PROPERTIES COMPILE_OPTIONS "")
# set_target_properties(TestAlpacaCoreSwift PROPERTIES
#     MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
# )

# target_compile_options(TestAlpacaCoreSwift PRIVATE -parse-as-library)

# target_include_directories(ac-swift-test PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

# add_executable(ac-swift-test t-Dict.swift)
# # From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# # so we clear them here

# target_link_libraries(ac-swift-test
#   PRIVATE
#       AlpacaCoreSwift
# )

# target_compile_options(ac-swift-test
#   PUBLIC
#     $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
# )

# target_include_directories(ac-swift-test
#   PRIVATE
#     ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CxxAlpacaCore
#     ${CMAKE_SOURCE_DIR}/wrapper/swift/test
# )

# target_compile_options(ac-swift-test PRIVATE -parse-as-library)
