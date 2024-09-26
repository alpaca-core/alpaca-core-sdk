# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-c-local-dummy AC_C_LOCAL_DUMMY
    ac/local_dummy.h
    ac/local_dummy.cpp
)
add_library(ac-c::local-dummy ALIAS ac-c-local-dummy)
target_include_directories(ac-c-local-dummy INTERFACE .)
target_link_libraries(ac-c-local-dummy PRIVATE
    ac::local-dummy
    ac-c::local
)
