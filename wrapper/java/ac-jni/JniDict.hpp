// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "jni.hpp"
#include <ac/Dict.hpp>

namespace ac::java {

using Obj = jni::Object<>;

jni::Local<Obj> Dict_toObject(jni::JNIEnv& env, const Dict& dict);
Dict Object_toDict(jni::JNIEnv& env, jni::Local<Obj> obj);

} // namespace ac::java
