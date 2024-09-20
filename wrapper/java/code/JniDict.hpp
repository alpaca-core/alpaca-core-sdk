// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "jni.hpp"
#include <ac/Dict.hpp>

namespace ac::java {

jni::Local<jni::Object<>> Dict_toMap(jni::JNIEnv& env, const Dict& map);

struct MapTag {
    static constexpr auto Name() { return "java/util/Map"; }
};
using Map = jni::Object<MapTag>;

Dict Map_toDict(jni::JNIEnv& env, jni::Local<Map> map);

} // namespace ac::java
