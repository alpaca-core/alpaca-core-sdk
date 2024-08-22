# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

icm_add_lib(ac-audio AC_AUDIO)

add_library(ac-dev::audio ALIAS ac-audio)
target_include_directories(ac-audio
    INTERFACE
        code
)
target_link_libraries(ac-audio
    PUBLIC
        splat::splat
)
target_sources(ac-audio
    PRIVATE
        code/ac-audio.cpp
)
