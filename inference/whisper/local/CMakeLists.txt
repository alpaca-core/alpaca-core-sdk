# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

icm_add_lib(ac-local-whisper AC_LOCAL_WHISPER)
add_library(ac::local-whisper ALIAS ac-local-whisper)

target_include_directories(ac-local-whisper PUBLIC code)

target_sources(ac-local-whisper PRIVATE
    code/ac/local/export.h
    code/ac/local/LocalWhisper.hpp
    code/ac/local/LocalWhisper.cpp
)

target_link_libraries(ac-local-whisper
    PRIVATE
        ac::whisper
        ac::local
)

ac_add_tools_subdir()
ac_add_example_subdir()
