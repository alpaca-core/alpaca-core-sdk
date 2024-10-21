# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-cocoa STATIC)

target_sources(ac-cocoa PRIVATE
    ac/DictConverter.hpp
    ac/DictConverter.mm
)

target_include_directories(ac-cocoa
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ac-cocoa
    PUBLIC
        ac::dict
)
