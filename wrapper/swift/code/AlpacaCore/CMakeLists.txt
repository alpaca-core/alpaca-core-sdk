# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

add_library(AlpacaCore STATIC DummyACapi.swift)

# From the Root CMAKE there are C++ compile options which conflict with Swift compiler
# so we clear them here
set_target_properties(AlpacaCore PROPERTIES COMPILE_OPTIONS "")

# Add C++ interop capability to the Swift target
# check if we're going to use the bridging header
set(CMAKE_Swift_FLAGS "-cxx-interoperability-mode=default")
set(CMAKE_Swift_FLAGS "${CMAKE_Swift_FLAGS} -import-objc-header ${CMAKE_SOURCE_DIR}/wrapper/swift/code/CAlpacaCore/CAlpacaCore-Bridging-Header.h" )

# Link the Payload Obj-C++ wrapper library
target_link_libraries(AlpacaCore PUBLIC CAlpacaCore)
