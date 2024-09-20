// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniDict.hpp"
#include <memory>

namespace ac::java {

namespace {

struct DictToMapCache {
    jni::JNIEnv& env;
    jni::Local<jni::Class<HashMapTag>> cls;
    jni::Constructor<HashMapTag> ctor;

    DictToMapCache(jni::JNIEnv& env)
        : env(env)
        , cls(jni::Class<HashMapTag>::Find(env))
        , ctor(cls.GetConstructor(env))
    {}
};


} // namespace

jni::Local<HashMap> Dict_toMap(jni::JNIEnv& env, const Dict& map) {
    DictToMapCache c(env);

    return ret;
}

Dict Map_toDict(jni::JNIEnv& env, jni::Local<Map> map) {
    return {};
}

} // namespace ac::java
