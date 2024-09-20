// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "jni.hpp"
#include <ac/Dict.hpp>

namespace ac::java {

struct MapTag {
    static constexpr auto Name() { return "java/util/Map"; }
};
using Map = jni::Object<MapTag>;

struct HashMapTag {
    static constexpr auto Name() { return "java/util/HashMap"; }
};
using HashMap = jni::Object<HashMapTag>;

jni::Local<HashMap> Dict_toMap(jni::JNIEnv& env, const Dict& map);

Dict Map_toDict(jni::JNIEnv& env, jni::Local<Map> map);

} // namespace ac::java
