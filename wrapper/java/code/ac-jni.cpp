#include "jni.hpp"
#include <iostream>

class Adder {
public:
    static constexpr auto Name() { return "com/alpacacore/api/Adder"; }

    jni::jint m_i;
    Adder(JNIEnv&, jint i) : m_i(i) {
        //std::cout << "Created Adder\n";
    }
    ~Adder() {
        //std::cout << "Deleted Adder\n";
    }

    jni::jint add(JNIEnv&, jni::jint a, jni::jint b) {
        return m_i + a + b;
    }

    void seti(JNIEnv&, jni::jint i) {
        m_i = i;
    }

    //jni::Local<jni::Object<Adder>> clone(JNIEnv& env) {
    //    jni::Local<jni::Object<Adder>> result(env, std::make_unique<Adder>(env, m_i));
    //}
};

#define METHOD(name, MethodPtr) jni::MakeNativePeerMethod<decltype(MethodPtr), MethodPtr>(name)

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void*) {
    auto& env = jni::GetEnv(*vm);

    jni::RegisterNativePeer<Adder>(env, jni::Class<Adder>::Find(env), "nativePtr"
        , jni::MakePeer<Adder, jni::jint>
        , "initialize"
        , "finalize"
        , METHOD("add", &Adder::add)
        , METHOD("seti", &Adder::seti)
        //, METHOD("clone", &Adder::clone)
    );

    return JNI_VERSION_1_6;
}
