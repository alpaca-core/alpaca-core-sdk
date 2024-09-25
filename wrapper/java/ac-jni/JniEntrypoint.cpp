#include "jni.hpp"
#include "JniSandbox.hpp"
#include "JniApi.hpp"

#include <jalog/Instance.hpp>
#include <jalog/sinks/DefaultSink.hpp>

#include <iostream>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void*) {
    jalog::Instance jl;
    jl.setup().add<jalog::sinks::DefaultSink>();

    auto& env = jni::GetEnv(*jvm);

    ac::java::JniSandbox_register(env);
    ac::java::JniApi_register(*jvm, env);

    return JNI_VERSION_1_6;
}
