# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(example-ac-local-dummy e-local-dummy.cpp)
target_link_libraries(example-ac-local-dummy PRIVATE
    ac::local
    ac::local-dummy
    ac::jalog
    ac-test-data::dummy
)
set_target_properties(example-ac-local-dummy PROPERTIES FOLDER example)
