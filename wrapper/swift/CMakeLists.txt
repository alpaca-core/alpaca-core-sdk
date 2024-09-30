# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
project (AlpacaCoreSwiftWrapper
    LANGUAGES Swift OBJCXX CXX
    )

add_subdirectory(code)
ac_add_example_subdir()
ac_add_test_subdir()
