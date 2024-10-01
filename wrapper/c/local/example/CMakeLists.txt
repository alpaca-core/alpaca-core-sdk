# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(add_local_example name)
    add_executable(example-ac-c-local-${name} e-local-${name}.c)
    target_link_libraries(example-ac-c-local-${name} PRIVATE
        ac-c::local
        ac-c::jalogc
        ac-test-data::${name}
    )
    set_target_properties(example-ac-c-local-${name} PROPERTIES FOLDER example)
endmacro()

add_local_example(dummy)

ac_dep(test-data-llama)
add_local_example(llama)
