# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#


# DummyExample =================
add_executable(DummyExample DummyExample.swift)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(DummyExample PROPERTIES COMPILE_OPTIONS "")

target_compile_options(DummyExample PUBLIC
  $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
  $<$<COMPILE_LANGUAGE:Swift>:-import-objc-header ${CMAKE_SOURCE_DIR}/wrapper/swift/util/DummyInference-Bridging-Header.h>
)

target_link_libraries(DummyExample
  PRIVATE
      CAlpacaCore
      ac-local-dummy
      DummyInference
)

target_compile_options(DummyExample PRIVATE -parse-as-library)

# CxxACExample =================
add_executable(CxxACExample CxxACExample.swift)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(CxxACExample PROPERTIES COMPILE_OPTIONS "")
target_link_libraries(CxxACExample
  PRIVATE
      CxxAlpacaCore
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

# CxxDummyExample =================

add_executable(CxxDummyExample CxxDummyExample.swift)
# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(CxxDummyExample PROPERTIES COMPILE_OPTIONS "")
target_link_libraries(CxxDummyExample
  PRIVATE
      CxxAlpacaCore
      ac-local
      ac-local-dummy
)

target_compile_options(CxxDummyExample
  PUBLIC
    $<$<COMPILE_LANGUAGE:Swift>:-cxx-interoperability-mode=default -Xcc -std=c++20>
)

target_include_directories(CxxDummyExample
  PUBLIC
    ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CxxAlpacaCore
    ${CMAKE_SOURCE_DIR}/wrapper/swift/example
)

target_compile_options(CxxDummyExample PRIVATE -parse-as-library)
