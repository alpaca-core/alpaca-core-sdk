# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(test-data-llama)

add_executable(ac-sandbox sandbox.cpp)
target_link_libraries(ac-sandbox
    ac::local
    ac::llama
    ac::local-llama
    jalog::jalog
    jalog::sinklib
    ac-test-data::llama
)
