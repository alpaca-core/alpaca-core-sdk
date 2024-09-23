// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniApi.hpp"
#include <ac/ModelDesc.hpp>
#include <iostream>

namespace ac::java {

struct ModelDesc {
    static constexpr auto Name() { return "com/alpacacore/api/ModelDesc"; }

    struct AssetInfo {
        static constexpr auto Name() { return "com/alpacacore/api/ModelDesc$AssetInfo"; }
    };

    static ac::ModelDesc get(JNIEnv& env, jni::Object<ModelDesc>& obj) {
        ac::ModelDesc ret;

        auto cls = jni::Class<ModelDesc>::Find(env);
        auto inferenceTypeField = cls.GetField<jni::String>(env, "inferenceType");
        ret.inferenceType = jni::Make<std::string>(env, obj.Get(env, inferenceTypeField));

        auto assetInfoClass = jni::Class<AssetInfo>::Find(env);
        auto assetInfoNameField = assetInfoClass.GetField<jni::String>(env, "path");
        auto assetInfoTagField = assetInfoClass.GetField<jni::String>(env, "tag");

        auto assetsField = cls.GetField<jni::Array<jni::Object<AssetInfo>>>(env, "assets");
        auto assetsArray = obj.Get(env, assetsField);
        auto assetsArrayLength = assetsArray.Length(env);
        ret.assets.reserve(assetsArrayLength);
        for (size_t i = 0; i < assetsArrayLength; ++i) {
            auto assetInfo = assetsArray.Get(env, i);
            auto name = jni::Make<std::string>(env, assetInfo.Get(env, assetInfoNameField));
            auto tag = jni::Make<std::string>(env, assetInfo.Get(env, assetInfoTagField));
            ret.assets.push_back({name, tag});
        }

        auto nameField = cls.GetField<jni::String>(env, "name");
        ret.name = jni::Make<std::string>(env, obj.Get(env, nameField));

        return ret;
    }
};

struct LocalProvider {
    static constexpr auto Name() { return "com/alpacacore/api/LocalProvider"; }

    static void sandbox(jni::JNIEnv& env, jni::Class<LocalProvider>&, jni::Object<ModelDesc>& jdesc) {
        auto desc = ModelDesc::get(env, jdesc);
        std::cout << "desc: " << desc.name << '\n';
        std::cout << "  inferenceType: " << desc.inferenceType << '\n';
        for (const auto& asset : desc.assets) {
            std::cout << "  asset: " << asset.path << " tag: " << asset.tag << '\n';
        }
    }
};

#define jniMakeNativeMethod(cls, mthd) jni::MakeNativeMethod<decltype(&cls::mthd), &cls::mthd>(#mthd)

void JniApi_register(jni::JNIEnv& env) {
    auto lpc = jni::Class<LocalProvider>::Find(env);
    jni::RegisterNatives(env, *lpc,
        jniMakeNativeMethod(LocalProvider, sandbox)
    );
}

} // namespace ac::java
