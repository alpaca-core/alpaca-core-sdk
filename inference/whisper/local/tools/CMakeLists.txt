# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
set(tgt ac-local-whisper-generate-schema)
add_executable(${tgt} generate-schema.cpp)
target_link_libraries(${tgt} PRIVATE
    ac::local
    ac::local-whisper
)
