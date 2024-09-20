// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniDict.hpp"
#include <memory>

DISABLE_MSVC_WARNING(4996) // codecvt deprecations

namespace ac::java {

namespace {

struct HashMapTag {
    static constexpr auto Name() { return "java/util/HashMap"; }
};
using HashMap = jni::Object<HashMapTag>;

struct BooleanTag {
    static constexpr auto Name() { return "java/lang/Boolean"; }
    using cpp_t = jboolean;
};
struct IntegerTag {
    static constexpr auto Name() { return "java/lang/Integer"; }
    using cpp_t = jint;
};
struct LongTag {
    static constexpr auto Name() { return "java/lang/Long"; }
    using cpp_t = jlong;
};
struct DoubleTag {
    static constexpr auto Name() { return "java/lang/Double"; }
    using cpp_t = jdouble;
};

template <typename Tag>
struct PrimitiveTypeCache {
    using cpp_t = typename Tag::cpp_t;
    jni::Local<jni::Class<Tag>> cls;
    std::optional<jni::Constructor<Tag, cpp_t>> ctor;

    PrimitiveTypeCache() : cls(nullptr) {}

    jni::Local<jni::Object<Tag>> create(jni::JNIEnv& env, cpp_t val) {
        if (!cls) {
            cls = jni::Class<Tag>::Find(env);
            ctor.emplace(cls.template GetConstructor<cpp_t>(env));
        }
        return cls.New(env, *ctor, val);
    }
};

struct BasicConverter {
    jni::JNIEnv& env;

    jni::Local<jni::Class<HashMapTag>> hashMapClass;
    jni::Constructor<HashMapTag> hashMapCtor;

    BasicConverter(jni::JNIEnv& env)
        : env(env)
        , hashMapClass(jni::Class<HashMapTag>::Find(env))
        , hashMapCtor(hashMapClass.GetConstructor<>(env))
    {}
};

struct DictToMapConverter : BasicConverter {
    jni::Method<HashMapTag, jni::Object<>(jni::Object<>, jni::Object<>)> m_hashMapPut;

    PrimitiveTypeCache<BooleanTag> boolCache;
    PrimitiveTypeCache<IntegerTag> intCache;
    PrimitiveTypeCache<LongTag> longCache;
    PrimitiveTypeCache<DoubleTag> doubleCache;

    DictToMapConverter(jni::JNIEnv& env)
        : BasicConverter(env)
        , m_hashMapPut(hashMapClass.GetMethod<jni::Object<>(jni::Object<>, jni::Object<>)>(env, "put"))
    {}

    jni::Local<HashMap> newHashMap() {
        return hashMapClass.New(env, hashMapCtor);
    }

    jni::Local<jni::Object<>> convert(const Dict& dict) {
        switch (dict.type()) {
        case Dict::value_t::boolean:
            return boolCache.create(env, dict.get<bool>());
        case Dict::value_t::number_integer:
            return intCache.create(env, dict.get<int>());
        case Dict::value_t::number_unsigned: {
            auto uval = dict.get<unsigned>();
            if (uval <= unsigned(std::numeric_limits<jint>::max())) {
                // return int if it fits
                return intCache.create(env, uval);
            }
            return longCache.create(env, uval);
        }
        case Dict::value_t::number_float:
            return doubleCache.create(env, dict.get<double>());
        case Dict::value_t::string: {
            auto sv = dict.get<std::string_view>();
            auto begin = sv.data();
            auto end = sv.data() + sv.size();
            return jni::Make<jni::String>(env,
                std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(begin, end));
        }
        case Dict::value_t::array: {
            auto ret = jni::Local<jni::Array<jni::Object<>>>::New(env, dict.size());
            jsize i = 0;
            for (auto& v : dict) {
                ret.Set(env, i++, convert(v));
            }
            return ret;
        }
        case Dict::value_t::object: {
            auto ret = newHashMap();
            for (auto& [k, v] : dict.items()) {
                auto keyObj = jni::Make<jni::String>(env, k);
                ret.Call(env, m_hashMapPut, keyObj, convert(v));
            }
            return ret;
        }
        case Dict::value_t::null:
        default:
            return jni::Local<jni::Object<>>(nullptr);
        }
    }
};

struct MapToDictConverter : BasicConverter {
    MapToDictConverter(jni::JNIEnv& env)
        : BasicConverter(env)
    {}
};

} // namespace

jni::Local<jni::Object<>> Dict_toMap(jni::JNIEnv& env, const Dict& dict) {
    return DictToMapConverter(env).convert(dict);
}

//Dict Map_toDict(jni::JNIEnv& env, jni::Local<Map> map) {
//    return {};
//}

} // namespace ac::java
