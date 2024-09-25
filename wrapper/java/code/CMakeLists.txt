# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#

# have this as a static lib so it can be tested in isolation
add_library(ac-jni-dict STATIC
    JniDict.hpp
    JniDict.cpp
)
set_target_properties(ac-jni-dict PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(ac-jni-dict INTERFACE .)
target_link_libraries(ac-jni-dict PUBLIC
    JNI::JNI
    mapbox::jni.hpp
    ac::api
)

# main module
add_library(ac-jni MODULE
    ac-jni.cpp
    JniSandbox.cpp
    JniApi.cpp
)
if(NOT WIN32)
    target_compile_options(ac-jni PRIVATE
        -fvisibility=hidden
        -fvisibility-inlines-hidden
    )
endif()
target_link_libraries(ac-jni PRIVATE
    ac-jni-dict
    ac::local
    ac::local-dummy
    ac::local-llama
    ac::local-whisper
)

add_jar(alpaca-core-jar
    SOURCES
        Sandbox.java
        Instance.java
        Model.java
        ModelDesc.java
        LocalProvider.java
        Impl.java
    OUTPUT_NAME
        alpaca-core
)
