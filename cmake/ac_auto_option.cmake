# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(ac_auto_option var desc default)
    set(${var} ${default} CACHE STRING ${desc})
    set_property(CACHE ${var} PROPERTY STRINGS AUTO ON OFF)
    if(${${var}} STREQUAL AUTO)
        set(${var}_REQUIRED)
    elseif(${${var}} STREQUAL ON)
        set(${var}_REQUIRED REQUIRED)
    elseif(${${var}} STREQUAL OFF)
        set(${var}_REQUIRED)
    else()
        message(FATAL_ERROR "Invalid value for ${var}: ${${var}}")
    endif()
endmacro()
