// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "jni.hpp"
#include <ac/Dict.hpp>
#include <memory>

// how do we handle references to the underlying C++ object?
// we could employ the C api and have distict refs and roots, but this is not idicomatic Java and will be very
// unusual to Java programmers
// So instead we'll use a shared_ptr to manage the lifetime of the root and have a refernce to the concrete item here

class JniDict {
public:
    static constexpr auto Name() { return "com/alpacacore/api/Dict"; }

    std::shared_ptr<ac::Dict> m_root;
    ac::Dict& m_dict; // points somewhere within m_root

    JniDict(JNIEnv&)
        : m_root(std::make_shared<ac::Dict>())
        , m_dict(*m_root)
    {}


};

void JniDict_register(jni::JNIEnv& env) {
    jni::RegisterNativePeer<JniDict>(env, jni::Class<JniDict>::Find(env), "nativePtr"
        , jni::MakePeer<JniDict>
        , "initialize"
        , "finalize"
    );
}
