# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(example-ac-local-llama e-local-llama.cpp)
target_link_libraries(example-ac-local-llama PRIVATE
    ac::local
    ac::local-llama
    jalog::jalog
    jalog::sinklib
    ac-test-data::llama
)
set_target_properties(example-ac-local-llama PROPERTIES FOLDER example)

add_executable(example-ac-c-local-llama e-local-llama.c)
target_link_libraries(example-ac-c-local-llama PRIVATE
    ac::local
    ac::local-llama
    ac::jalogc
    ac-test-data::llama
)
set_target_properties(example-ac-c-local-llama PROPERTIES FOLDER example)
if(MSVC)
    target_compile_options(example-ac-c-local-llama PRIVATE /experimental:c11atomics)
endif()
