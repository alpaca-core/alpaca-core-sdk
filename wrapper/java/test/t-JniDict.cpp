// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "jni.hpp"
#include "JniDict.hpp"

extern "C" {

JNIEXPORT jni::jobject* JNICALL
Java_com_alpacacore_api_TestDict_getTestMapFromDict(jni::JNIEnv* env, jni::jclass* /*cls*/) {
    return ac::java::Dict_toMap(*env, {}).release();
}

JNIEXPORT void JNICALL
Java_com_alpacacore_api_TestDict_runCppTestWithMap(JNIEnv* /*env*/, jni::jclass* /*cls*/, jni::jobject* /*map*/) {
}

} // extern "C"
