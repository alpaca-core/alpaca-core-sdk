# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
add_library(ac-jni MODULE
    ac-jni.cpp
)
target_link_libraries(ac-jni
    PUBLIC
        JNI::JNI
)

add_jar(alpaca-core-java
    SOURCES
        Adder.java
    OUTPUT_NAME
        alpaca-core
)
