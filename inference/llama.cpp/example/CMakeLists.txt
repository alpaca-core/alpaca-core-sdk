# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

add_executable(ac-example-llama-basic e-basic.cpp)
target_link_libraries(ac-example-llama-basic PRIVATE
    ac::llama
    ac-test-data::llama
    jalog::jalog
    jalog::sinklib
)
set_target_properties(ac-example-llama-basic PROPERTIES FOLDER example)
