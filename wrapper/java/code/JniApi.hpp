// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "jni.hpp"

namespace ac::java {
void JniApi_register(jni::JavaVM& jvm, jni::JNIEnv& env);
}
