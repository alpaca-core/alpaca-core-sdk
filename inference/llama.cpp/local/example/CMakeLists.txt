# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(test-data-llama)

function(add_local_llama_example name)
    set(tgt example-ac-local-llama-${name})
    add_executable(${tgt} e-local-llama-${name}.cpp)
    target_link_libraries(${tgt} PRIVATE
        ac::local
        ac::local-llama
        ac::jalog
        ac-test-data::llama
    )
    set_target_properties(${tgt} PROPERTIES FOLDER example)
endfunction()

add_local_llama_example(run)
add_local_llama_example(chat)
