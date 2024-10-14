# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

add_library(cocoa-wrapper STATIC)

target_sources(cocoa-wrapper PRIVATE
    ac/Dict.h
    ac/Dict.mm

)

target_link_libraries(cocoa-wrapper
    PUBLIC
        ac::dict
)

target_include_directories(cocoa-wrapper
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
