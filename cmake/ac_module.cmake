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
    if(APPLE)
        # by default on macOS the default suffix by cmake for modules is .so
        # this doesn't play well with many loaders which require .dylib
        # instead of changing this globally and potentially breaking third
        # party code, we'll only do this for our own modules
        set_target_properties(${name} PROPERTIES SUFFIX ".dylib")
    endif()
endmacro()
