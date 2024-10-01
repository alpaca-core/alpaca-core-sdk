# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

add_library(CAlpacaCore STATIC)

target_sources(CAlpacaCore PRIVATE
    ./AlpacaCore.h
    ./AlpacaCore.mm
    ./Dict.h
    ./Dict.mm
    ./Model.h
    ./Model.mm
    ./Instance.h
    ./Instance.mm
)

target_link_libraries(CAlpacaCore
    PUBLIC
        ac::dict
        ac::local
)

target_include_directories(CAlpacaCore
    PUBLIC
        ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CAlpacaCore
        ${CMAKE_SOURCE_DIR}/local/ac/local
)
