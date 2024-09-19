# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
CPMAddPackage(gh:mapbox/jni.hpp@4.0.0)
add_library(mapbox-jni.hpp INTERFACE)
add_library(mapbox::jni.hpp ALIAS mapbox-jni.hpp)
target_include_directories(mapbox-jni.hpp INTERFACE ${jni.hpp_SOURCE_DIR}/include)

add_library(ac-jni MODULE
    ac-jni.cpp
    JniDict.cpp
)
if(NOT WIN32)
    target_compile_options(ac-jni PRIVATE
        -fvisibility=hidden
        -fvisibility-inlines-hidden
    )
endif()
target_link_libraries(ac-jni
    PRIVATE
        JNI::JNI
        mapbox::jni.hpp
        ac::api
)

add_jar(alpaca-core-java
    SOURCES
        Adder.java
        Dict.java
    OUTPUT_NAME
        alpaca-core
)
