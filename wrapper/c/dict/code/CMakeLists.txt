# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-c-dict AC_C_DICT)
add_library(ac-c::dict ALIAS ac-c-dict)
target_include_directories(ac-c-dict INTERFACE .)
target_link_libraries(ac-c-dict
    PUBLIC
        splat::splat
        ac-c::util
    PRIVATE
        ac::dict
        ac::astl-private
)
target_sources(ac-c-dict PRIVATE
    ac/export.h
    ac/dict_ref.h
    ac/dict_root.h
    ac/dict.h
    ac/dict.cpp
)
