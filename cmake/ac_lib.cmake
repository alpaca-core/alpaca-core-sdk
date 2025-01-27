# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
include_guard(GLOBAL)

function(add_ac_lib target cname)
    if(AC_STATIC)
        add_library(${target} STATIC ${ARGN})
    else()
        icm_add_shared_lib(${target} ${cname} ${ARGN})
    endif()
endfunction()
