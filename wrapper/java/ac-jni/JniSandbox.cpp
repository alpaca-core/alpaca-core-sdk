// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniSandbox.hpp"

namespace ac::java {

namespace {

class Sandbox {
public:
    static constexpr auto Name() { return "com/alpacacore/Sandbox"; }

    jni::jint m_i;
    Sandbox(JNIEnv&, jint i) : m_i(i) {
        //std::cout << "Created Sandbox\n";
    }
    ~Sandbox() {
        //std::cout << "Deleted Sandbox\n";
    }

    jni::jint add(JNIEnv&, jni::jint a, jni::jint b) {
        return m_i + a + b;
    }

    void seti(JNIEnv&, jni::jint i) {
        m_i = i;
    }

    jni::Local<jni::Object<Sandbox>> clone(JNIEnv& env) {
        auto cls = jni::Class<Sandbox>::Find(env);
        auto m = cls.GetStaticMethod<jni::Object<Sandbox>(jlong)>(env, "fromNative");
        auto ptr = std::make_unique<Sandbox>(env, m_i);
        auto ret = cls.Call(env, m, reinterpret_cast<jlong>(ptr.get()));
        ptr.release();
        return ret;
    }
};

} // namespace

#define METHOD(name, MethodPtr) jni::MakeNativePeerMethod<decltype(MethodPtr), MethodPtr>(name)

void JniSandbox_register(jni::JNIEnv& env) {
    jni::RegisterNativePeer<Sandbox>(env, jni::Class<Sandbox>::Find(env), "nativePtr"
        , jni::MakePeer<Sandbox, jni::jint>
        , "initialize"
        , "finalize"
        , METHOD("add", &Sandbox::add)
        , METHOD("seti", &Sandbox::seti)
        , METHOD("clone", &Sandbox::clone)
    );
}

} // namespace ac::java
