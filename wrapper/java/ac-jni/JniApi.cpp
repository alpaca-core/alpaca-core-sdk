// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniApi.hpp"
#include "JniDict.hpp"

#include <ac/LocalDummy.hpp>
#include <ac/LocalLlama.hpp>
#include <ac/LocalWhisper.hpp>

#include <ac/ModelDesc.hpp>
#include <ac/LocalProvider.hpp>

#include <ac/Model.hpp>
#include <ac/Instance.hpp>

#include <iostream>
#include <thread>

namespace ac::java {

struct ModelDesc {
    static constexpr auto Name() { return "com/alpacacore/ModelDesc"; }

    struct AssetInfo {
        static constexpr auto Name() { return "com/alpacacore/ModelDesc$AssetInfo"; }
    };

    static ac::ModelDesc get(JNIEnv& env, jni::Object<ModelDesc>& obj) {
        ac::ModelDesc ret;

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

struct LocalProviderSingleton : public ac::LocalProvider {
    LocalProviderSingleton() : ac::LocalProvider(ac::LocalProvider::No_LaunchThread) {}
    ~LocalProviderSingleton() {
        abortRun();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    std::thread m_thread;
    jni::JNIEnv* workerEnv = nullptr;

    void launch(jni::JavaVM& jvm) {
        m_thread = std::thread([this, &jvm] {
            auto env = jni::AttachCurrentThreadAsDaemon(jvm);
            workerEnv = env.get();
            run();
        });
    }
};

std::unique_ptr<LocalProviderSingleton> providerSingleton;

template <typename Tag, typename PL>
struct PrivateNativeClass {
    struct Payload {
        Payload(PL&& data) : data(std::move(data)) {}
        PL data;
    };

    static void finalize(jni::JNIEnv& env, jni::Object<Tag>& obj) {
        auto cls = jni::Class<Tag>::Find(env);
        auto payloadField = cls.template GetField<jni::jlong>(env, "nativePtr");
        auto payload = obj.Get(env, payloadField);
        auto ptr = reinterpret_cast<Payload*>(payload);
        delete ptr;
    }

    static jni::Local<jni::Object<Tag>> create(jni::JNIEnv& env, PL payloadData) {
        auto cls = jni::Class<Tag>::Find(env);
        auto ctor = cls.template GetConstructor<jni::jlong>(env);
        auto payload = std::make_unique<Payload>(std::move(payloadData));
        auto nativePtr = reinterpret_cast<jni::jlong>(payload.get());
        auto obj = cls.New(env, ctor, nativePtr);
        payload.release();
        return obj;
    }

    static PL& getPayload(jni::JNIEnv& env, jni::Object<Tag>& obj) {
        auto cls = jni::Class<Tag>::Find(env);
        auto payloadField = cls.template GetField<jni::jlong>(env, "nativePtr");
        auto payload = obj.Get(env, payloadField);
        auto ptr = reinterpret_cast<Payload*>(payload);
        return ptr->data;
    }
};

struct InstanceBase {
    static constexpr auto Name() { return "com/alpacacore/Instance"; }
};

struct InstanceImpl : public PrivateNativeClass<InstanceImpl, InstancePtr> {
    static constexpr auto Name() { return "com/alpacacore/InstanceImpl"; }
    using SuperTag = InstanceBase;

    static void synchronize(jni::JNIEnv& env, jni::Object<InstanceImpl>& obj) {
        auto& instance = getPayload(env, obj);
        instance->synchronize();
    }

    struct OpCallback {
        static constexpr auto Name() { return "com/alpacacore/Instance$OpCallback"; }

        jni::JNIEnv& env;
        jni::Global<jni::Object<OpCallback>> jcb;
        jni::Local<jni::Class<OpCallback>> cls;

        OpCallback(jni::JNIEnv& env, jni::Global<jni::Object<OpCallback>> jcb)
            : env(env)
            , jcb(std::move(jcb))
            , cls(jni::Class<OpCallback>::Find(env))
        {}

        void onComplete() {
            auto onComplete = cls.GetMethod<void()>(env, "onComplete");
            jcb.Call(env, onComplete);
        }

        void onError(std::string error) {
            auto onError = cls.GetMethod<void(jni::String)>(env, "onError");
            jcb.Call(env, onError, jni::Make<jni::String>(env, std::move(error)));
        }

        void onProgress(std::string_view tag, float progress) {
            auto onProgress = cls.GetMethod<void(jni::String, jni::jfloat)>(env, "onProgress");
            jcb.Call(env, onProgress, jni::Make<jni::String>(env, std::string(tag)), progress);
        }

        void onStream(Dict data) {
            auto onStream = cls.GetMethod<void(jni::Object<>)>(env, "onStream");
            jcb.Call(env, onStream, Dict_toObject(env, data));
        }
    };

    static void runOp(jni::JNIEnv& env, jni::Object<InstanceImpl>& obj, jni::String& op, jni::Object<>& params, jni::Object<OpCallback>& jcb) {
        auto& instance = getPayload(env, obj);

        auto paramsDict = Object_toDict(env, jni::NewLocal(env, params));

        instance->runOp(jni::Make<std::string>(env, op), std::move(paramsDict), {
            [jcb = jni::NewGlobal(env, jcb)](CallbackResult<void> result) mutable {
                OpCallback cb(*providerSingleton->workerEnv, std::move(jcb));
                if (result.has_value()) {
                    cb.onComplete();
                }
                else {
                    cb.onError(std::move(result.error().text));
                }
            },
            [jcb = jni::NewGlobal(env, jcb)](Dict data) mutable {
                OpCallback cb(*providerSingleton->workerEnv, std::move(jcb));
                cb.onStream(std::move(data));
            },
            [jcb = jni::NewGlobal(env, jcb)](std::string_view tag, float progress) mutable {
                OpCallback cb(*providerSingleton->workerEnv, std::move(jcb));
                cb.onProgress(tag, progress);
            }
        });
    }

    struct AbortCallback {
        static constexpr auto Name() { return "com/alpacacore/Instance$AbortCallback"; }

        jni::JNIEnv& env;
        jni::Global<jni::Object<AbortCallback>> jcb;
        jni::Local<jni::Class<AbortCallback>> cls;

        AbortCallback(jni::JNIEnv& env, jni::Global<jni::Object<AbortCallback>> jcb)
            : env(env)
            , jcb(std::move(jcb))
            , cls(jni::Class<AbortCallback>::Find(env))
        {}

        void onAbort() {
            auto onAbort = cls.GetMethod<void()>(env, "onAbort");
            jcb.Call(env, onAbort);
        }
    };

    static void initiateAbort(jni::JNIEnv& env, jni::Object<InstanceImpl>& obj, jni::Object<AbortCallback>& jcb) {
        auto& instance = getPayload(env, obj);

        instance->initiateAbort([jcb = jni::NewGlobal(env, jcb)]() mutable {
            AbortCallback cb(*providerSingleton->workerEnv, std::move(jcb));
            cb.onAbort();
        });
    }
};

struct ModelBase {
    static constexpr auto Name() { return "com/alpacacore/Model"; }
};

struct ModelImpl : public PrivateNativeClass<ModelImpl, ModelPtr> {
    static constexpr auto Name() { return "com/alpacacore/ModelImpl"; }
    using SuperTag = ModelBase;

    struct CreateInstanceCallback {
        constexpr static auto Name() { return "com/alpacacore/Model$CreateInstanceCallback"; }

        jni::JNIEnv& env;
        jni::Global<jni::Object<CreateInstanceCallback>> jcb;
        jni::Local<jni::Class<CreateInstanceCallback>> cls;

        CreateInstanceCallback(jni::JNIEnv& env, jni::Global<jni::Object<CreateInstanceCallback>> jcb)
            : env(env)
            , jcb(std::move(jcb))
            , cls(jni::Class<CreateInstanceCallback>::Find(env))
        {}

        void onComplete(InstancePtr instance) {
            auto obj = InstanceImpl::create(env, std::move(instance));
            auto onComplete = cls.GetMethod<void(jni::Object<InstanceBase>)>(env, "onComplete");
            jcb.Call(env, onComplete, obj);
        }

        void onError(std::string error) {
            auto onError = cls.GetMethod<void(jni::String)>(env, "onError");
            jcb.Call(env, onError, jni::Make<jni::String>(env, std::move(error)));
        }
    };

    static void createInstance(jni::JNIEnv& env, jni::Object<ModelImpl>& obj, jni::String& type, jni::Object<>& params, jni::Object<CreateInstanceCallback>& jcb) {
        auto& model = getPayload(env, obj);

        auto paramsDict = Object_toDict(env, jni::NewLocal(env, params));

        model->createInstance(jni::Make<std::string>(env, type), std::move(paramsDict), [jcb = jni::NewGlobal(env, jcb)](CallbackResult<InstancePtr> result) mutable {
            CreateInstanceCallback cb(*providerSingleton->workerEnv, std::move(jcb));
            if (result.has_value()) {
                cb.onComplete(std::move(*result));
            }
            else {
                cb.onError(std::move(result.error().text));
            }
        });
    }
};

struct LocalProvider {
    static constexpr auto Name() { return "com/alpacacore/LocalProvider"; }

    static void sandbox(jni::JNIEnv& env, jni::Class<LocalProvider>&, jni::Object<ModelDesc>& jdesc) {
        auto desc = ModelDesc::get(env, jdesc);
        std::cout << "desc: " << desc.name << '\n';
        std::cout << "  inferenceType: " << desc.inferenceType << '\n';
        for (const auto& asset : desc.assets) {
            std::cout << "  asset: " << asset.path << " tag: " << asset.tag << '\n';
        }
    }

    struct LoadModelCallback {
        constexpr static auto Name() { return "com/alpacacore/LocalProvider$LoadModelCallback"; }

        jni::JNIEnv& env;
        jni::Global<jni::Object<LoadModelCallback>> jcb;
        jni::Local<jni::Class<LoadModelCallback>> cls;
        LoadModelCallback(jni::JNIEnv& env, jni::Global<jni::Object<LoadModelCallback>> jcb)
            : env(env)
            , jcb(std::move(jcb))
            , cls(jni::Class<LoadModelCallback>::Find(env))
        {}

        void onComplete(ModelPtr model) {
            auto obj = ModelImpl::create(env, std::move(model));
            auto onComplete = cls.GetMethod<void(jni::Object<ModelBase>)>(env, "onComplete");
            jcb.Call(env, onComplete, obj);
        }

        void onError(std::string error) {
            auto onError = cls.GetMethod<void(jni::String)>(env, "onError");
            jcb.Call(env, onError, jni::Make<jni::String>(env, std::move(error)));
        }

        void onProgress(std::string_view tag, float progress) {
            auto onProgress = cls.GetMethod<void(jni::String, jni::jfloat)>(env, "onProgress");
            jcb.Call(env, onProgress, jni::Make<jni::String>(env, std::string(tag)), progress);
        }
    };

    static void loadModel(jni::JNIEnv& env, jni::Class<LocalProvider>&, jni::Object<ModelDesc>& jdesc, jni::Object<>& jparams, jni::Object<LoadModelCallback>& jcb) {
        auto desc = ModelDesc::get(env, jdesc);
        auto params = Object_toDict(env, jni::NewLocal(env, jparams));
        providerSingleton->createModel(std::move(desc), std::move(params), {
            [jcb = jni::NewGlobal(env, jcb)](CallbackResult<ModelPtr> result) mutable {
                LoadModelCallback cb(*providerSingleton->workerEnv, std::move(jcb));
                if (result.has_value()) {
                    cb.onComplete(std::move(*result));
                }
                else {
                    cb.onError(std::move(result.error().text));
                }
            },
            [jcb = jni::NewGlobal(env, jcb)](std::string_view tag, float progress) mutable {
                LoadModelCallback cb(*providerSingleton->workerEnv, std::move(jcb));
                cb.onProgress(tag, progress);
            }
        });
    }

    static void shutdown(jni::JNIEnv&, jni::Class<LocalProvider>&) {
        providerSingleton.reset();
    }
};

#define jniMakeNativeMethod(cls, mthd) jni::MakeNativeMethod<decltype(&cls::mthd), &cls::mthd>(#mthd)

void JniApi_register(jni::JavaVM& jvm, jni::JNIEnv& env) {
    auto ic = jni::Class<InstanceImpl>::Find(env);
    jni::RegisterNatives(env, *ic,
        jniMakeNativeMethod(InstanceImpl, synchronize),
        jniMakeNativeMethod(InstanceImpl, runOp),
        jniMakeNativeMethod(InstanceImpl, initiateAbort)
    );

    auto mc = jni::Class<ModelImpl>::Find(env);
    jni::RegisterNatives(env, *mc
        , jniMakeNativeMethod(ModelImpl, finalize)
        , jniMakeNativeMethod(ModelImpl, createInstance)
    );

    auto lpc = jni::Class<LocalProvider>::Find(env);
    jni::RegisterNatives(env, *lpc
        , jniMakeNativeMethod(LocalProvider, sandbox)
        , jniMakeNativeMethod(LocalProvider, loadModel)
        , jniMakeNativeMethod(LocalProvider, shutdown)
    );

    providerSingleton = std::make_unique<LocalProviderSingleton>();
    providerSingleton->launch(jvm);

    addLocalDummyInference(*providerSingleton);
    addLocalLlamaInference(*providerSingleton);
    addLocalWhisperInference(*providerSingleton);
}

} // namespace ac::java
