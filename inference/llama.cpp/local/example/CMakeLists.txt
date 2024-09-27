# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(test-data-llama)

add_executable(example-ac-local-llama e-local-llama.cpp)
target_link_libraries(example-ac-local-llama PRIVATE
    ac::local
    ac::local-llama
    jalog::jalog
    jalog::sinklib
    ac-test-data::llama
)
set_target_properties(example-ac-local-llama PROPERTIES FOLDER example)

