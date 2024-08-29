# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_executable(example-xec-coro e-xec-coro.cpp)
target_link_libraries(example-xec-coro PRIVATE
    xec::xec
    ac::astl
)
set_target_properties(example-xec-coro PROPERTIES FOLDER example)
