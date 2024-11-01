# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# add module: shared, not affected by BUILD_SHARED_LIBS
macro(ac_add_module name)
    add_library(${name} MODULE ${ARGN})
    if(NOT WIN32)
        target_compile_options(${name} PRIVATE
            -fvisibility=hidden
            -fvisibility-inlines-hidden
        )
    endif()
endmacro()
