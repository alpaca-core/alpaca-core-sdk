# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
ac_dep(test-data-whisper)

set(TARGET example-ac-whisper-basic)

add_executable(${TARGET} e-basic.cpp)
target_link_libraries(${TARGET} PRIVATE
    ac::whisper
    ac::jalog
    ac-test-data::whisper
    ac-dev::audio
)
set_target_properties(${TARGET} PROPERTIES FOLDER example)

if(TARGET ac-dev::imgui)
    set(TARGET example-ac-whisper-gui)
    add_executable(${TARGET} e-gui.cpp)
    target_link_libraries(${TARGET} PRIVATE
        ac::whisper
        ac::jalog
        ac-test-data::whisper
        ac-dev::audio
        ac-dev::imgui
    )
    set_target_properties(${TARGET} PROPERTIES FOLDER example)
endif()
