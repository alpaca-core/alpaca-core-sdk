#include "jni.hpp"
#include "JniSandbox.hpp"
#include "JniApi.hpp"
#include <iostream>

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    auto& env = jni::GetEnv(*vm);

    ac::java::JniSandbox_register(env);
    ac::java::JniApi_register(env);

    return JNI_VERSION_1_6;
}
