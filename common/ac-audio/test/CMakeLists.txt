# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(test-data-whisper)

add_doctest_lib_test(core ac-audio
SOURCES
    t-ac-audio.cpp
LIBRARIES
    ac-test-data::whisper
)
