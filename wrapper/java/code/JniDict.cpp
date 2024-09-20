// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniDict.hpp"
#include <memory>

DISABLE_MSVC_WARNING(4996) // codecvt deprecations

namespace ac::java {

using Obj = jni::Object<>;

namespace {

struct HashMapTag {
    static constexpr auto Name() { return "java/util/HashMap"; }
};
using HashMap = jni::Object<HashMapTag>;

struct BooleanTag {
    static constexpr auto Name() { return "java/lang/Boolean"; }
    using cpp_t = jboolean;
    static constexpr auto valueFunc = "booleanValue";
};
struct IntegerTag {
    static constexpr auto Name() { return "java/lang/Integer"; }
    using cpp_t = jint;
    static constexpr auto valueFunc = "intValue";
};
struct LongTag {
    static constexpr auto Name() { return "java/lang/Long"; }
    using cpp_t = jlong;
    static constexpr auto valueFunc = "longValue";
};
struct DoubleTag {
    static constexpr auto Name() { return "java/lang/Double"; }
    using cpp_t = jdouble;
    static constexpr auto valueFunc = "doubleValue";
};

template <typename Tag>
struct PrimitiveTypeCache {
    using cpp_t = typename Tag::cpp_t;
    jni::Local<jni::Class<Tag>> cls;
    std::optional<jni::Constructor<Tag, cpp_t>> ctor;
    std::optional<jni::Method<Tag, cpp_t()>> getter;

    PrimitiveTypeCache() : cls(nullptr) {}

    void initCls(jni::JNIEnv& env) {
        if (!cls) {
            cls = jni::Class<Tag>::Find(env);
        }
    }

    jni::Local<jni::Object<Tag>> create(jni::JNIEnv& env, cpp_t val) {
        if (!ctor) {
            initCls(env);
            ctor.emplace(cls.template GetConstructor<cpp_t>(env));
        }
        return cls.New(env, *ctor, val);
    }

    std::optional<cpp_t> get(jni::JNIEnv& env, const jni::Local<Obj>& obj) {
        initCls(env);

        if (!obj.IsInstanceOf(env, cls)) {
            return {};
        }

        if (!getter) {
            getter.emplace(cls.template GetMethod<cpp_t()>(env, Tag::valueFunc));
        }

        auto cast = jni::Cast<Tag>(env, cls, obj);

        return cast.Call(env, *getter);
    }
};

struct DictToMapConverter {
    jni::JNIEnv& env;
    jni::Local<jni::Class<HashMapTag>> hashMapClass;
    jni::Constructor<HashMapTag> hashMapCtor;
    jni::Method<HashMapTag, Obj(Obj, Obj)> m_hashMapPut;

    PrimitiveTypeCache<BooleanTag> boolCache;
    PrimitiveTypeCache<IntegerTag> intCache;
    PrimitiveTypeCache<LongTag> longCache;
    PrimitiveTypeCache<DoubleTag> doubleCache;

    DictToMapConverter(jni::JNIEnv& env)
        : env(env)
        , hashMapClass(jni::Class<HashMapTag>::Find(env))
        , hashMapCtor(hashMapClass.GetConstructor<>(env))
        , m_hashMapPut(hashMapClass.GetMethod<Obj(Obj, Obj)>(env, "put"))
    {}

    jni::Local<HashMap> newHashMap() {
        return hashMapClass.New(env, hashMapCtor);
    }

    jni::Local<Obj> convert(const Dict& dict) {
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
            auto ret = jni::Local<jni::Array<Obj>>::New(env, dict.size());
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
            return jni::Local<Obj>(nullptr);
        }
    }
};

struct MapToDictConverter {
    jni::JNIEnv& env;
    jni::Local<jni::Class<jni::StringTag>> stringClass;
    jni::Local<jni::Class<MapTag>> mapClass;
    jni::Local<jni::Class<jni::ArrayTag<Obj>>> objArrayClass;

    PrimitiveTypeCache<BooleanTag> boolCache;
    PrimitiveTypeCache<IntegerTag> intCache;
    PrimitiveTypeCache<LongTag> longCache;
    PrimitiveTypeCache<DoubleTag> doubleCache;

    MapToDictConverter(jni::JNIEnv& env)
        : env(env)
        , stringClass(jni::Class<jni::StringTag>::Find(env))
        , mapClass(jni::Class<MapTag>::Find(env))
        , objArrayClass(jni::Class<jni::ArrayTag<Obj>>::Find(env))
    {}

    bool getNull(const jni::Local<Obj>& obj) {
        return !obj;
    }

    std::optional<std::string> getString(const jni::Local<Obj>& obj) {
        if (!obj.IsInstanceOf(env, stringClass)) {
            return {};
        }

        auto str = jni::Cast<jni::StringTag>(env, stringClass, obj);

        return jni::Make<std::string>(env, str);
    }

    Dict getArray(const jni::Local<Obj>& obj) {
        if (!obj.IsInstanceOf(env, objArrayClass)) {
            return {};
        }

        auto cast = jni::Cast<jni::ArrayTag<Obj>>(env, objArrayClass, obj);

        return {};
    }

    Dict convert(jni::Local<Obj> obj) {
        if (getNull(obj)) {
            return {};
        }
        if (auto str = getString(obj)) {
            return *str;
        }
        if (auto b = boolCache.get(env, obj)) {
            return *b;
        }
        if (auto i = intCache.get(env, obj)) {
            return *i;
        }
        if (auto l = longCache.get(env, obj)) {
            // longs are special...
            // first try to fit into int
            // ... then into unsigned
            // ... finally into double
            return *l;
        }
        if (auto d = doubleCache.get(env, obj)) {
            return *d;
        }

    }
};

} // namespace

jni::Local<Obj> Dict_toMap(jni::JNIEnv& env, const Dict& dict) {
    return DictToMapConverter(env).convert(dict);
}

Dict Map_toDict(jni::JNIEnv& env, jni::Local<Map> map) {
    return {};
}

} // namespace ac::java
