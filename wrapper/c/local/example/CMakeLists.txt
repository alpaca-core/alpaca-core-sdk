# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(example-ac-c-local-dummy e-local-dummy.c)
target_link_libraries(example-ac-c-local-dummy PRIVATE
    ac-c::local
    ac-c::local-dummy
    ac-c::jalogc
    ac-test-data::dummy
)
set_target_properties(example-ac-local-dummy PROPERTIES FOLDER example)
