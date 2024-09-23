// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "JniApi.hpp"
#include <iostream>

namespace ac::java {

class LocalProvider {
public:
    static constexpr auto Name() { return "com/alpacacore/api/LocalProvider"; }

    static void sandbox(jni::JNIEnv& env, jni::Class<LocalProvider>&, jni::String& jstr) {
        auto str = jni::Make<std::string>(env, jstr);
        std::cout << "java said: " << str << "\n";
    }
};


void JniApi_register(jni::JNIEnv& env) {
    auto lpc = jni::Class<LocalProvider>::Find(env);
    jni::RegisterNatives(env, *lpc,
        jni::MakeNativeMethod<decltype(&LocalProvider::sandbox), &LocalProvider::sandbox>("sandbox")
    );
}

} // namespace ac::java
