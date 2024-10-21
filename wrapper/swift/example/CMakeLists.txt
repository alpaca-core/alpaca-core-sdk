# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

macro(ac_add_swift_executable target)
    add_executable(${target} ${ARGN})
    set_target_properties(${target} PROPERTIES COMPILE_OPTIONS "")
    target_link_libraries(${target}
      PRIVATE
        AlpacaCoreSwift
    )
    target_compile_options(${target}
      PUBLIC
        $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
    )
    target_include_directories(${target}
      PUBLIC
        ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CxxAlpacaCore
        ${CMAKE_SOURCE_DIR}/wrapper/swift/example
    )
    target_compile_options(${target} PRIVATE -parse-as-library)
endmacro()

ac_add_swift_executable(CxxACExample CxxACExample.swift)
ac_add_swift_executable(WhisperExample e-Whisper.swift)
ac_add_swift_executable(DummyExample e-Dummy.swift)
