# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# CxxACExample =================
add_executable(CxxACExample CxxACExample.swift)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(CxxACExample PROPERTIES COMPILE_OPTIONS "")
target_link_libraries(CxxACExample
  PRIVATE
    AlpacaCoreSwift
)

target_compile_options(CxxACExample
  PUBLIC
    $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
)

target_include_directories(CxxACExample
  PUBLIC
    ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CxxAlpacaCore
)

target_compile_options(CxxACExample PRIVATE -parse-as-library)


# WhisperExample =================

add_executable(WhisperExample e-Whisper.swift)
# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(WhisperExample PROPERTIES COMPILE_OPTIONS "")
target_link_libraries(WhisperExample
  PRIVATE
      AlpacaCoreSwift
)

target_compile_options(WhisperExample
  PUBLIC
    $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
)

target_include_directories(WhisperExample
  PUBLIC
    ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CxxAlpacaCore
    ${CMAKE_SOURCE_DIR}/wrapper/swift/example
)

target_compile_options(WhisperExample PRIVATE -parse-as-library)

# DummyExample =================

add_executable(DummyExample e-Dummy.swift)
# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(DummyExample PROPERTIES COMPILE_OPTIONS "")
target_link_libraries(DummyExample
  PRIVATE
      AlpacaCoreSwift
)

target_compile_options(DummyExample
  PUBLIC
    $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
)

target_include_directories(DummyExample
  PUBLIC
    ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CxxAlpacaCore
    ${CMAKE_SOURCE_DIR}/wrapper/swift/example
)

target_compile_options(DummyExample PRIVATE -parse-as-library)
