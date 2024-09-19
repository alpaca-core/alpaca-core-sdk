#include <splat/warnings.h>

PRAGMA_WARNING_PUSH
DISABLE_MSVC_WARNING(4624) // deleted destructor
DISABLE_MSVC_WARNING(4996) // codecvt deprecations
#include <jni/jni.hpp>
PRAGMA_WARNING_POP

#include <iostream>

class Adder {
public:
    static constexpr auto Name() { return "com/alpacacore/api/Adder"; }

    jni::jint m_i;
    Adder(JNIEnv&, jint i) : m_i(i) {}

    jni::jint add(JNIEnv&, jni::jint a, jni::jint b) {
        return m_i + a + b;
    }
};

#define METHOD(name, MethodPtr) jni::MakeNativePeerMethod<decltype(MethodPtr), MethodPtr>(name)

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    auto& env = jni::GetEnv(*vm);

    jni::RegisterNativePeer<Adder>(env, jni::Class<Adder>::Find(env), "nativePtr"
        , jni::MakePeer<Adder, jni::jint>
        , "initialize"
        , "finalize"
        , METHOD("add", &Adder::add)
    );

    return JNI_VERSION_1_6;
}
