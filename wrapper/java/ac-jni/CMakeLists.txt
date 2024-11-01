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
    ac::dict
)

# main module
ac_add_module(ac-jni
    JniEntrypoint.cpp
    JniApi.cpp
)
target_link_libraries(ac-jni PRIVATE
    ac-jni-dict
    ac::local
    ac::local-dummy
    ac::local-llama
    ac::local-whisper
    jalog::jalog
    jalog::sinklib
)
