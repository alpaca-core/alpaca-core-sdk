// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniDict.hpp"
#include <astl/throw_ex.hpp>
#include <memory>

DISABLE_MSVC_WARNING(4996) // codecvt deprecations

namespace ac::java {

namespace {

struct HashMapTag {
    static constexpr auto Name() { return "java/util/HashMap"; }
};
using HashMap = jni::Object<HashMapTag>;

struct MapTag {
    static constexpr auto Name() { return "java/util/Map"; }
};
using Map = jni::Object<MapTag>;

struct BooleanTag {
    static constexpr auto Name() { return "java/lang/Boolean"; }
    using cpp_t = jboolean;
    static constexpr auto valueFuncName = "booleanValue";
};
struct IntegerTag {
    static constexpr auto Name() { return "java/lang/Integer"; }
    using cpp_t = jint;
    static constexpr auto valueFuncName = "intValue";
};
struct LongTag {
    static constexpr auto Name() { return "java/lang/Long"; }
    using cpp_t = jlong;
    static constexpr auto valueFuncName = "longValue";
};
struct DoubleTag {
    static constexpr auto Name() { return "java/lang/Double"; }
    using cpp_t = jdouble;
    static constexpr auto valueFuncName = "doubleValue";
};

template <class OutTagType, class T>
jni::Local<jni::Object<OutTagType>> unsafeCast(jni::JNIEnv& env, const T& object)
{
    auto ptr = reinterpret_cast<typename jni::Object<OutTagType>::UntaggedType*>(jni::NewLocal(env, object).release());
    return jni::Local<jni::Object<OutTagType>>(env, ptr);
}

template <typename Tag>
struct PrimitiveClass {
    jni::JNIEnv& env;

    using cpp_t = typename Tag::cpp_t;

    jni::Local<jni::Class<Tag>> cls;
    jni::Constructor<Tag, cpp_t> ctor;
    jni::Method<Tag, cpp_t()> valueFunc;

    PrimitiveClass(jni::JNIEnv& env)
        : env(env),
        cls(jni::Class<Tag>::Find(env)),
        ctor(cls.template GetConstructor<cpp_t>(env)),
        valueFunc(cls.template GetMethod<cpp_t()>(env, Tag::valueFuncName))
    {}

    struct Instance {
        const PrimitiveClass& parent;
        jni::Local<jni::Object<Tag>> obj;

        cpp_t value() const {
            return obj.Call(parent.env, parent.valueFunc);
        }
    };

    Instance newInstance(cpp_t value) const {
        return {*this, cls.New(env, ctor, value)};
    }

    Instance cast(jni::Local<Obj> obj) const {
        return {
            *this,
            unsafeCast<Tag>(env, std::move(obj))
        };
    }

    std::optional<cpp_t> safeGet(jni::Local<Obj>& obj) const {
        if (!obj.IsInstanceOf(env, cls)) {
            return {};
        }

        return cast(std::move(obj)).value();
    }
};

using BooleanClass = PrimitiveClass<BooleanTag>;
using IntegerClass = PrimitiveClass<IntegerTag>;
using LongClass = PrimitiveClass<LongTag>;
using DoubleClass = PrimitiveClass<DoubleTag>;

struct DictToMapConverter {
    jni::JNIEnv& env;
    jni::Local<jni::Class<HashMapTag>> hashMapClass;
    jni::Constructor<HashMapTag> hashMapCtor;
    jni::Method<HashMapTag, Obj(Obj, Obj)> m_hashMapPut;

    BooleanClass boolCls;
    IntegerClass intCls;
    LongClass longCls;
    DoubleClass doubleCls;

    DictToMapConverter(jni::JNIEnv& env)
        : env(env)
        , hashMapClass(jni::Class<HashMapTag>::Find(env))
        , hashMapCtor(hashMapClass.GetConstructor<>(env))
        , m_hashMapPut(hashMapClass.GetMethod<Obj(Obj, Obj)>(env, "put"))
        , boolCls(env)
        , intCls(env)
        , longCls(env)
        , doubleCls(env)
    {}

    jni::Local<HashMap> newHashMap() {
        return hashMapClass.New(env, hashMapCtor);
    }

    jni::Local<Obj> convert(const Dict& dict) {
        switch (dict.type()) {
        case Dict::value_t::boolean:
            return boolCls.newInstance(dict.get<bool>()).obj;
        case Dict::value_t::number_integer:
            return intCls.newInstance(dict.get<int>()).obj;
        case Dict::value_t::number_unsigned: {
            auto uval = dict.get<unsigned>();
            if (uval <= unsigned(std::numeric_limits<jint>::max())) {
                // return int if it fits
                return intCls.newInstance(uval).obj;
            }
            return longCls.newInstance(uval).obj;
        }
        case Dict::value_t::number_float:
            return doubleCls.newInstance(dict.get<double>()).obj;
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

struct SetTag {
    static constexpr auto Name() { return "java/util/Set"; }
};

struct MapToDictConverter {
    jni::JNIEnv& env;
    jni::Local<jni::Class<jni::StringTag>> stringClass;
    jni::Local<jni::Class<jni::ArrayTag<Obj>>> objArrayClass;

    jni::Local<jni::Class<MapTag>> mapClass;
    //jni::Method

    BooleanClass boolCls;
    IntegerClass intCls;
    LongClass longCls;
    DoubleClass doubleCls;

    MapToDictConverter(jni::JNIEnv& env)
        : env(env)
        , stringClass(jni::Class<jni::StringTag>::Find(env))
        , mapClass(jni::Class<MapTag>::Find(env))
        , objArrayClass(jni::Class<jni::ArrayTag<Obj>>::Find(env))
        , boolCls(env)
        , intCls(env)
        , longCls(env)
        , doubleCls(env)
    {}

    bool getNull(const jni::Local<Obj>& obj) {
        return !obj;
    }

    std::optional<std::string> safeGetString(const jni::Local<Obj>& obj) {
        if (!obj.IsInstanceOf(env, stringClass)) {
            return {};
        }

        auto str = unsafeCast<jni::StringTag>(env, obj);
        return jni::Make<std::string>(env, str);
    }

    Dict getArray(const jni::Local<Obj>& obj) {
        if (!obj.IsInstanceOf(env, objArrayClass)) {
            return {};
        }

        auto cast = jni::Cast<jni::ArrayTag<Obj>>(env, objArrayClass, obj);
        jni::Local<jni::Array<Obj>> arr(env, cast.release()); // silly secondary cast since Array is not Object :(

        auto size = arr.Length(env);

        Dict ret = Dict::array();

        if (size == 0) return ret; // empty

        ret[size - 1] = {}; // hacky reserve

        for (jsize i = 0; i < size; ++i) {
            ret[i] = convert(arr.Get(env, i));
        }

        return ret;
    }

    Dict getObject(const jni::Local<Obj>& obj) {
        if (!obj.IsInstanceOf(env, mapClass)) {
            return {};
        }

        auto map = unsafeCast<MapTag>(env, obj);

        Dict ret;

        return ret;
    }

    Dict convert(jni::Local<Obj> obj) {
        if (getNull(obj)) {
            return {};
        }
        if (auto str = safeGetString(obj)) {
            return *str;
        }
        if (auto b = boolCls.safeGet(obj)) {
            return *b;
        }
        if (auto i = intCls.safeGet(obj)) {
            return *i;
        }
        if (auto l = longCls.safeGet(obj)) {
            // longs are special...
            // first try to fit into int
            // ... then into unsigned
            // ... finally into double
            return *l;
        }
        if (auto d = doubleCls.safeGet(obj)) {
            return *d;
        }
        if (auto arr = getArray(obj)) {
            return arr;
        }

        throw std::runtime_error("Unsupported type");
    }
};

} // namespace

jni::Local<Obj> Dict_toObject(jni::JNIEnv& env, const Dict& dict) {
    return DictToMapConverter(env).convert(dict);
}

Dict Object_toDict(jni::JNIEnv& env, jni::Local<Obj> obj) {
    return MapToDictConverter(env).convert(std::move(obj));
}

} // namespace ac::java
