#include "jni.hpp"
#include "JniApi.hpp"

#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>

#include <iostream>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void*) {
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    auto& env = jni::GetEnv(*jvm);

    ac::java::JniApi_register(*jvm, env);

    return JNI_VERSION_1_6;
}
