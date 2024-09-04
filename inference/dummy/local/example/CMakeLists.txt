# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(example-ac-local-dummy e-local-dummy.cpp)
target_link_libraries(example-ac-local-dummy PRIVATE
    ac::local
    ac::local-dummy
    jalog::jalog
    jalog::sinklib
    ac-test-data::dummy
)
set_target_properties(example-ac-local-dummy PROPERTIES FOLDER example)

add_executable(example-ac-c-local-dummy e-local-dummy.c)
target_link_libraries(example-ac-c-local-dummy PRIVATE
    ac::local
    ac::local-dummy
    ac::jalogc
    ac-test-data::dummy
)
set_target_properties(example-ac-c-local-dummy PROPERTIES FOLDER example)
if(MSVC)
    target_compile_options(example-ac-c-local-dummy PRIVATE /experimental:c11atomics)
endif()
