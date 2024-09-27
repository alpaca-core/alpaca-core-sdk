// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniApi.hpp"
#include "JniDict.hpp"

#include <ac/local/LocalDummy.hpp>
#include <ac/local/LocalLlama.hpp>
#include <ac/local/LocalWhisper.hpp>

#include <ac/local/ModelDesc.hpp>
#include <ac/local/ModelFactory.hpp>

#include <ac/local/Model.hpp>
#include <ac/local/Instance.hpp>

#include <astl/move.hpp>
#include <itlib/make_ptr.hpp>
#include <iostream>

namespace ac::java {

struct ModelDesc {
    static constexpr auto Name() { return "com/alpacacore/ModelDesc"; }

    struct AssetInfo {
        static constexpr auto Name() { return "com/alpacacore/ModelDesc$AssetInfo"; }
    };

    static local::ModelDesc get(JNIEnv& env, jni::Object<ModelDesc>& obj) {
        local::ModelDesc ret;

        auto cls = jni::Class<ModelDesc>::Find(env);
        auto inferenceTypeField = cls.GetField<jni::String>(env, "inferenceType");
        ret.inferenceType = jni::Make<std::string>(env, obj.Get(env, inferenceTypeField));

        auto assetInfoClass = jni::Class<AssetInfo>::Find(env);
        auto assetInfoPathField = assetInfoClass.GetField<jni::String>(env, "path");
        auto assetInfoTagField = assetInfoClass.GetField<jni::String>(env, "tag");

        auto assetsField = cls.GetField<jni::Array<jni::Object<AssetInfo>>>(env, "assets");
        auto assetsArray = obj.Get(env, assetsField);

        // treat null as empty

        if (assetsArray) {
            auto assetsArrayLength = assetsArray.Length(env);
            ret.assets.reserve(assetsArrayLength);
            for (size_t i = 0; i < assetsArrayLength; ++i) {
                auto assetInfo = assetsArray.Get(env, i);
                auto& asset = ret.assets.emplace_back();
                asset.path = jni::Make<std::string>(env, assetInfo.Get(env, assetInfoPathField));
                if (auto jtag = assetInfo.Get(env, assetInfoTagField)) {
                    asset.tag = jni::Make<std::string>(env, jtag);
                }
            }
        }

        auto nameField = cls.GetField<jni::String>(env, "name");
        if (auto jname = obj.Get(env, nameField)) {
            ret.name = jni::Make<std::string>(env, jname);
        }

        return ret;
    }
};

template <typename Tag, typename PL>
struct NativeClass {
    const jni::Class<Tag>& cls;
    jni::Field<Tag, jni::jlong> nativePtrField;
    jni::Constructor<Tag> ctor;

    NativeClass(jni::JNIEnv& env)
        : cls(jni::Class<Tag>::Singleton(env))
        , nativePtrField(cls.GetField<jni::jlong>(env, "nativePtr"))
        , ctor(cls.GetConstructor<>(env))
    {}

    PL* getPtr(jni::JNIEnv& env, jni::Object<Tag>& obj) {
        auto payload = obj.Get(env, nativePtrField);
        return reinterpret_cast<PL*>(payload);
    }

    void setPtr(jni::JNIEnv& env, jni::Object<Tag>& obj, PL* payload) {
        obj.Set(env, nativePtrField, reinterpret_cast<jlong>(payload));
    }

    jni::Local<jni::Object<Tag>> create(JNIEnv& env, std::unique_ptr<PL> payload) {
        auto ret = cls.New(env, ctor);
        setPtr(env, ret, payload.get());
        payload.release();
        return ret;
    }

    void release(JNIEnv& env, jni::Object<Tag>& obj) {
        delete getPtr(env, obj);
        setPtr(env, obj, nullptr);
    }
};

struct ProgressCallback {
    constexpr static auto Name() { return "com/alpacacore/ProgressCallback"; }

    jni::JNIEnv& env;
    jni::Object<ProgressCallback>& pcb;
    const jni::Class<ProgressCallback>& cls;
    jni::Method<ProgressCallback, jni::jboolean(jni::String, jni::jfloat)> onProgressMethod;

    ProgressCallback(jni::JNIEnv& env, jni::Object<ProgressCallback>& pcb)
        : env(env)
        , pcb(pcb)
        , cls(jni::Class<ProgressCallback>::Singleton(env))
        , onProgressMethod(cls.GetMethod<jni::jboolean(jni::String, jni::jfloat)>(env, "onProgress"))
    {}

    bool onProgress(std::string_view tag, float progress) const {
        return !!pcb.Call(env, onProgressMethod, jni::Make<jni::String>(env, std::string(tag)), progress);
    }

    static local::ProgressCb makeProgressCb(jni::JNIEnv& env, jni::Object<ProgressCallback>& pcb) {
        if (!pcb) return {};
        return [cb = ProgressCallback(env, pcb)](std::string_view tag, float progress) {
            return cb.onProgress(tag, progress);
        };
    }
};

struct Instance {
    static constexpr auto Name() { return "com/alpacacore/Instance"; }

    static std::optional<NativeClass<Instance, local::Instance>> nativeClass;

    static jni::Local<jni::Object<>> runOp(jni::JNIEnv& env, jni::Object<Instance>& obj, jni::String& op, jni::Object<>& params, jni::Object<ProgressCallback>& pcb) {
        auto instance = nativeClass->getPtr(env, obj);

        auto paramsDict = Object_toDict(env, jni::NewLocal(env, params));

        auto cb = ProgressCallback::makeProgressCb(env, pcb);
        auto result = instance->runOp(jni::Make<std::string>(env, op), astl::move(paramsDict), astl::move(cb));
        return Dict_toObject(env, result);
    }
};

std::optional<NativeClass<Instance, local::Instance>> Instance::nativeClass;

struct Model {
    static constexpr auto Name() { return "com/alpacacore/Model"; }

    struct Payload {
        local::ModelPtr model;
    };

    static std::optional<NativeClass<Model, Payload>> nativeClass;

    static jni::Local<jni::Object<Instance>> createInstance(jni::JNIEnv& env, jni::Object<Model>& obj, jni::String& type, jni::Object<>& params) {
        auto& model = nativeClass->getPtr(env, obj)->model;

        auto paramsDict = Object_toDict(env, jni::NewLocal(env, params));

        auto instance = model->createInstance(jni::Make<std::string>(env, type), astl::move(paramsDict));

        return Instance::nativeClass->create(env, astl::move(instance));
    }
};

std::optional<NativeClass<Model, Model::Payload>> Model::nativeClass;

std::unique_ptr<local::ModelFactory> factorySingleton;

struct AlpacaCore {
    static constexpr auto Name() { return "com/alpacacore/AlpacaCore"; }

    static jni::Local<jni::Object<Model>> createModel(
        jni::JNIEnv& env,
        jni::Class<AlpacaCore>&,
        jni::Object<ModelDesc>& desc,
        jni::Object<>& params,
        jni::Object<ProgressCallback>& pcb
    ) {
        auto model = factorySingleton->createModel(
            ModelDesc::get(env, desc),
            Object_toDict(env, jni::NewLocal(env, params)),
            ProgressCallback::makeProgressCb(env, pcb)
        );
        return Model::nativeClass->create(env, itlib::make_unique(Model::Payload{model}));
    }

    static void releaseModel(jni::JNIEnv& env, jni::Class<AlpacaCore>&, jni::Object<Model>& obj) {
        if (!obj) return;
        Model::nativeClass->release(env, obj);
    }

    static void releaseInstance(jni::JNIEnv& env, jni::Class<AlpacaCore>&, jni::Object<Instance>& obj) {
        if (!obj) return;
        Instance::nativeClass->release(env, obj);
    }
};

#define jniMakeNativeMethod(cls, mthd) jni::MakeNativeMethod<decltype(&cls::mthd), &cls::mthd>(#mthd)

void JniApi_register(jni::JavaVM&, jni::JNIEnv& env) {
    auto& ic = Instance::nativeClass.emplace(env);
    jni::RegisterNatives(env, *ic.cls
        , jniMakeNativeMethod(Instance, runOp)
    );

    auto& mc = Model::nativeClass.emplace(env);
    jni::RegisterNatives(env, *mc.cls
        , jniMakeNativeMethod(Model, createInstance)
    );

    auto acc = jni::Class<AlpacaCore>::Find(env);
    jni::RegisterNatives(env, *acc
        , jniMakeNativeMethod(AlpacaCore, createModel)
        , jniMakeNativeMethod(AlpacaCore, releaseModel)
        , jniMakeNativeMethod(AlpacaCore, releaseInstance)
    );

    factorySingleton = std::make_unique<local::ModelFactory>();

    local::addDummyInference(*factorySingleton);
    local::addLlamaInference(*factorySingleton);
    local::addWhisperInference(*factorySingleton);
}

} // namespace ac::java
