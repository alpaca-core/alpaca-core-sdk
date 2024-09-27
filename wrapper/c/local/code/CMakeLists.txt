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
        ac::local-dummy
        ac::local-llama
)
target_sources(ac-c-local PRIVATE
    ac/export.h

    ac/local.h
    ac/local.cpp

    ac/local_dummy.h
    ac/local_dummy.cpp

    ac/local_llama.h
    ac/local_llama.cpp
)
