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
