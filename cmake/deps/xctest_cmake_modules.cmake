# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
CPMAddPackage(
    NAME xctest_cmake_modules
    VERSION 1.0
    GIT_REPOSITORY https://github.com/hylo-lang/CMakeModules
    GIT_TAG 6577fca51495e82a94e76a8b70113c9e6b98f539
)
list(APPEND CMAKE_MODULE_PATH
    "${xctest_cmake_modules_SOURCE_DIR}"
)
