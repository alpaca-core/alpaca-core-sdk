#include "jni.hpp"
#include "JniSandbox.hpp"
#include "JniApi.hpp"
#include <iostream>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, void*) {
    auto& env = jni::GetEnv(*jvm);

    ac::java::JniSandbox_register(env);
    ac::java::JniApi_register(*jvm, env);

    return JNI_VERSION_1_6;
}
