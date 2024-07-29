# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

add_executable(example-ac-llama-basic e-basic.cpp)
target_link_libraries(example-ac-llama-basic PRIVATE
    ac::llama
    ac-test-data::llama
    jalog::jalog
    jalog::sinklib
)
set_target_properties(example-ac-llama-basic PROPERTIES FOLDER example)
