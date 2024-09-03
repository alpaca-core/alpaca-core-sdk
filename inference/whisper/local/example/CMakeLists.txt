# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(test-data-whisper)

add_executable(example-ac-local-whisper e-local-whisper.cpp)
target_link_libraries(example-ac-local-whisper
    PRIVATE
        ac::local
        ac::local-whisper
        jalog::jalog
        jalog::sinklib
        ac-test-data::whisper
        ac-dev::audio
)
set_target_properties(example-ac-local-whisper PROPERTIES FOLDER example)

