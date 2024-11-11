# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
function(add_ac_local_lib target cname)
    if(AC_LOCAL_STATIC)
        add_library(${target} STATIC ${ARGN})
    else()
        icm_add_shared_lib(${target} ${cname} ${ARGN})
    endif()
endfunction()
