# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(ac_audio_test test)
    add_doctest_lib_test(${test} ac-audio
        SOURCES
            t-${test}.cpp
        LIBRARIES
            ac-test-data::whisper
    )
endmacro()

ac_audio_test(integration)
