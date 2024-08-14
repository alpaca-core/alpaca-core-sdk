# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
icm_add_lib(ac-whisper AC_WHISPER)
add_library(ac::whisper ALIAS ac-whisper)
target_include_directories(ac-whisper INTERFACE .)
target_link_libraries(ac-whisper
    PRIVATE
        whisper
        jalog::jalog
    PUBLIC
        splat::splat
        ac::astl
)
target_sources(ac-whisper PRIVATE
    ac/whisper/export.h
    ac/whisper/Init.hpp
    ac/whisper/Init.cpp
    ac/whisper/Logging.hpp
    ac/whisper/Logging.cpp
    ac/whisper/Model.hpp
    ac/whisper/Model.cpp
    ac/whisper/Instance.hpp
    ac/whisper/Instance.cpp
)
