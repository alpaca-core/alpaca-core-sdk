# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(test-data-llama)

add_executable(example-ac-llama-basic e-basic.cpp)
target_link_libraries(example-ac-llama-basic PRIVATE
    ac::llama
    ac-test-data::llama
    ac::jalog
)
set_target_properties(example-ac-llama-basic PROPERTIES FOLDER example)

if(TARGET ac-dev::imgui)
    add_executable(example-ac-llama-gui e-gui.cpp)
    target_link_libraries(example-ac-llama-gui PRIVATE
        ac::llama
        ac-test-data::llama
        ac::jalog
        ac-dev::imgui
    )
    set_target_properties(example-ac-llama-gui PROPERTIES FOLDER example)
endif()
