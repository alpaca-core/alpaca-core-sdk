# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
dep(xec)
add_executable(poc-xec-coro poc-xec-coro.cpp)
target_link_libraries(poc-xec-coro PRIVATE
    xec::xec
    ac::astl
)
set_target_properties(poc-xec-coro PROPERTIES FOLDER poc)
