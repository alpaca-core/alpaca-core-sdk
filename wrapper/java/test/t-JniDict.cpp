// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "jni.hpp"
#include "JniDict.hpp"

extern "C" {


JNIEXPORT jni::jobject* JNICALL
Java_com_alpacacore_api_TestDict_getObjectFromDictByJson(jni::JNIEnv* env, jni::jclass* /*cls*/, jni::jstring* jjson) {
    auto json = jni::Make<std::string>(*env, jni::Local<jni::String>(*env, jjson));
    if (json.empty()) {
        return ac::java::Dict_toObject(*env, {}).release();
    }
    auto dict = ac::Dict::parse(std::move(json));
    return ac::java::Dict_toObject(*env, dict).release();
}

JNIEXPORT jboolean JNICALL
Java_com_alpacacore_api_TestDict_runCppTestWithNullObject(jni::JNIEnv* env, jni::jclass* /*cls*/, jni::jobject* obj) try {
    auto dict = ac::java::Object_toDict(*env, jni::Local<ac::java::Obj>(*env, obj));
    return dict.is_null();
}
catch (...) {
    jni::ThrowJavaError(*env, std::current_exception());
    return false; // reachable by C++, but not by Java
}

JNIEXPORT jboolean JNICALL
Java_com_alpacacore_api_TestDict_runCppTestWithPojoObject(JNIEnv* env, jni::jclass* /*cls*/, jni::jobject* obj) try {
    auto dict = ac::java::Object_toDict(*env, jni::Local<ac::java::Obj>(*env, obj));
    return true;
}
catch (...) {
    jni::ThrowJavaError(*env, std::current_exception());
    return false; // reachable by C++, but not by Java
}

} // extern "C"
