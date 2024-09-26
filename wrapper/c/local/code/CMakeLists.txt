# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-c-local AC_C_LOCAL)
add_library(ac-c::local ALIAS ac-c-local)
target_include_directories(ac-c-local INTERFACE .)
target_link_libraries(ac-c-local
    PUBLIC
        splat::splat
        ac-c::dict
    PRIVATE
        ac::local
)
target_sources(ac-c-local PRIVATE
    ac/local.h
    ac/local.cpp
)
