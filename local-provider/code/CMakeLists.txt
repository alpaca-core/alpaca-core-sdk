# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-local AC_API)
add_library(ac::local ALIAS ac-local)
target_include_directories(ac-local INTERFACE .)
target_link_libraries(ac-local
    PUBLIC
        ac::api
)
target_sources(ac-local PRIVATE
    ac/local/export.h
)
