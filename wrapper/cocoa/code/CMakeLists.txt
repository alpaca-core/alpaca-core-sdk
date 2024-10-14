# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-cocoa STATIC)

target_sources(ac-cocoa PRIVATE
    ac/Dict.h
    ac/Dict.mm
)

target_link_libraries(ac-cocoa
    PUBLIC
        ac::dict
)
