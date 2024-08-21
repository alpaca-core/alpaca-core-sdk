# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(TARGET example-ac-whisper-basic)

add_executable(${TARGET} e-basic.cpp)
target_link_libraries(${TARGET} PRIVATE
    ac::whisper
    jalog::jalog
    jalog::sinklib
    ac-test-data::whisper
    ac-dev::audio
)
set_target_properties(${TARGET} PROPERTIES FOLDER example)
