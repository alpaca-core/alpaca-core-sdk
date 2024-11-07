# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# plugin utilities

function(add_ac_local_plugin tname)
    add_library(${tname} MODULE
        ${ARGN}
    )
    if(NOT WIN32)
        target_compile_options(${tname} PRIVATE
            -fvisibility=hidden
            -fvisibility-inlines-hidden
        )
    endif()
    target_link_libraries(${tname} PRIVATE
        ac::local
        ac::jalog
    )
endfunction()
