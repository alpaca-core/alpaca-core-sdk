# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(test-data-llama)

add_executable(ac-sandbox sandbox.cpp)
target_link_libraries(ac-sandbox
    ac::http
)
