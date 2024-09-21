// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "jni.hpp"
#include "JniDict.hpp"

extern "C" {

JNIEXPORT jni::jobject* JNICALL
Java_com_alpacacore_api_TestDict_getObjectFromEmptyDict(jni::JNIEnv* env, jni::jclass* /*cls*/) {
    return ac::java::Dict_toObject(*env, {}).release();
}

JNIEXPORT jni::jobject* JNICALL
Java_com_alpacacore_api_TestDict_getObjectFromPojoDict(jni::JNIEnv* env, jni::jclass* /*cls*/) {
    auto dict = ac::Dict::parse(R"json(
    {
        "bool": true,
        "int": 42,
        "inner": {
            "str": "hello",
            "float": 3.14,
            "neg": -100,
            "big": 3000000000
        },
        "inner2": {
            "str": "world",
            "ilist": [1, "two", null, false],
            "null": null
        },
        "empty_list": [],
        "empty_dict": {}
    }
    )json");
    return ac::java::Dict_toObject(*env, dict).release();
}

//JNIEXPORT void JNICALL
//Java_com_alpacacore_api_TestDict_runCppTestWithPojoObject(JNIEnv* /*env*/, jni::jclass* /*cls*/, jni::jobject* /*map*/) {
//}

} // extern "C"
