// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "JniDict.hpp"
#include <astl/throw_ex.hpp>
#include <itlib/generator.hpp>
#include <memory>

DISABLE_MSVC_WARNING(4996) // codecvt deprecations

namespace ac::java {

namespace {

struct HashMapClass {
    jni::JNIEnv& env;

    struct Tag {
        static constexpr auto Name() { return "java/util/HashMap"; }
    };

    jni::Local<jni::Class<Tag>> cls;
    jni::Constructor<Tag> ctor;
    jni::Method<Tag, Obj(Obj, Obj)> putFunc;

    HashMapClass(jni::JNIEnv& env)
        : env(env)
        , cls(jni::Class<Tag>::Find(env)),
        ctor(cls.GetConstructor<>(env)),
        putFunc(cls.GetMethod<Obj(Obj, Obj)>(env, "put"))
    {}

    struct Instance {
        HashMapClass& parent;
        jni::Local<jni::Object<Tag>> obj;

        void put(jni::Local<Obj> key, jni::Local<Obj> value) {
            obj.Call(parent.env, parent.putFunc, key, value);
        }
    };

    Instance newInstance() {
        return { *this, cls.New(env, ctor) };
    }
};

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

    HashMapClass hashMapCls;
    BooleanClass boolCls;
    IntegerClass intCls;
    LongClass longCls;
    DoubleClass doubleCls;

    DictToMapConverter(jni::JNIEnv& env)
        : env(env)
        , hashMapCls(env)
        , boolCls(env)
        , intCls(env)
        , longCls(env)
        , doubleCls(env)
    {}

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
            auto ret = hashMapCls.newInstance();
            for (auto& [k, v] : dict.items()) {
                ret.put(
                    jni::Make<jni::String>(env, k),
                    convert(v)
                );
            }
            return std::move(ret.obj);
        }
        case Dict::value_t::null:
        default:
            return jni::Local<Obj>(nullptr);
        }
    }
};

struct MapClass {
    jni::JNIEnv& env;

    struct Tag {
        static constexpr auto Name() { return "java/util/Map"; }
    };

    jni::Local<jni::Class<Tag>> cls;

    struct SetTag {
        static constexpr auto Name() { return "java/util/Set"; }
    };
    jni::Local<jni::Class<SetTag>> setCls;

    struct IteratorTag {
        static constexpr auto Name() { return "java/util/Iterator"; }
    };
    jni::Local<jni::Class<IteratorTag>> iteratorCls;

    struct MapEntryTag {
        static constexpr auto Name() { return "java/util/Map$Entry"; }
    };
    jni::Local<jni::Class<MapEntryTag>> mapEntryCls;

    jni::Method<Tag, jni::Object<SetTag>()> entrySetFunc;
    jni::Method<SetTag, jni::Object<IteratorTag>()> iteratorFunc;
    jni::Method<IteratorTag, jboolean()> hasNextFunc;
    jni::Method<IteratorTag, Obj()> nextFunc;
    jni::Method<MapEntryTag, Obj()> getKeyFunc;
    jni::Method<MapEntryTag, Obj()> getValueFunc;

    MapClass(jni::JNIEnv& env)
        : env(env)
        , cls(jni::Class<Tag>::Find(env))
        , setCls(jni::Class<SetTag>::Find(env))
        , iteratorCls(jni::Class<IteratorTag>::Find(env))
        , mapEntryCls(jni::Class<MapEntryTag>::Find(env))
        , entrySetFunc(cls.GetMethod<jni::Object<SetTag>()>(env, "entrySet"))
        , iteratorFunc(setCls.GetMethod<jni::Object<IteratorTag>()>(env, "iterator"))
        , hasNextFunc(iteratorCls.GetMethod<jboolean()>(env, "hasNext"))
        , nextFunc(iteratorCls.GetMethod<Obj()>(env, "next"))
        , getKeyFunc(mapEntryCls.GetMethod<Obj()>(env, "getKey"))
        , getValueFunc(mapEntryCls.GetMethod<Obj()>(env, "getValue"))
    {}

    struct Instance {
        const MapClass& parent;
        jni::Local<jni::Object<Tag>> obj;

        struct Kv {
            jni::Local<Obj> key;
            jni::Local<Obj> value;
        };

        itlib::generator<Kv&> items() {
            auto& penv = parent.env;
            auto entrySet = obj.Call(penv, parent.entrySetFunc);
            auto iterator = entrySet.Call(penv, parent.iteratorFunc);
            while (iterator.Call(penv, parent.hasNextFunc)) {
                auto entry = unsafeCast<MapEntryTag>(penv, iterator.Call(penv, parent.nextFunc));
                Kv y = {
                    entry.Call(penv, parent.getKeyFunc),
                    entry.Call(penv, parent.getValueFunc)
                };
                co_yield y;
            }
        }
    };

    Instance cast(jni::Local<Obj> obj) const {
        return {
            *this,
            unsafeCast<Tag>(env, std::move(obj))
        };
    }
};

struct MapToDictConverter {
    jni::JNIEnv& env;
    jni::Local<jni::Class<jni::StringTag>> stringClass;
    jni::Local<jni::Class<jni::ArrayTag<Obj>>> objArrayClass;

    MapClass mapCls;
    BooleanClass boolCls;
    IntegerClass intCls;
    LongClass longCls;
    DoubleClass doubleCls;

    MapToDictConverter(jni::JNIEnv& env)
        : env(env)
        , stringClass(jni::Class<jni::StringTag>::Find(env))
        , objArrayClass(jni::Class<jni::ArrayTag<Obj>>::Find(env))
        , mapCls(env)
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

    std::optional<Dict> safeGetArray(const jni::Local<Obj>& obj) {
        if (!obj.IsInstanceOf(env, objArrayClass)) {
            return {};
        }

        auto cast = jni::Cast<jni::ArrayTag<Obj>>(env, objArrayClass, obj);
        jni::Local<jni::Array<Obj>> arr(env, cast.release()); // silly secondary cast since Array is not Object :(

        auto size = arr.Length(env);

        Dict ret = Dict::array();

        if (size == 0) return ret; // empty

        ret[size - 1] = {}; // hacky reserve

        for (jni::jsize i = 0; i < size; ++i) {
            ret[i] = convert(arr.Get(env, i));
        }

        return ret;
    }

    std::optional<Dict> getObject(jni::Local<Obj>& obj) {
        if (!obj.IsInstanceOf(env, mapCls.cls)) {
            return {};
        }

        auto map = mapCls.cast(std::move(obj));

        Dict ret;

        for (auto& item : map.items()) {
            auto key = safeGetString(item.key);
            if (!key) {
                throw std::runtime_error("Unsupported key type");
            }

            ret[*key] = convert(std::move(item.value));
        }

        return ret;
    }

    Dict convert(jni::Local<Obj> obj) {
        if (getNull(obj)) {
            return {};
        }
        if (auto str = safeGetString(obj)) {
            return std::move(*str);
        }
        if (auto b = boolCls.safeGet(obj)) {
            return !!*b;
        }
        if (auto i = intCls.safeGet(obj)) {
            return *i;
        }
        if (auto l = longCls.safeGet(obj)) {
            auto val = *l;
            // longs are special...

            // first try to fit into int
            if (val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                return int(val);
            }

            // ... then into unsigned
            if (val >= 0 && val <= std::numeric_limits<unsigned>::max()) {
                return unsigned(val);
            }

            // ... finally into double
            // json imposed limits (max integer which can be stored in a double)
            static constexpr int64_t Max_JsonInt64 = 9007199254740992ll;
            if (val >= -Max_JsonInt64 && val <= Max_JsonInt64) {
                return double(val);
            }

            throw std::runtime_error("long value too large");
        }
        if (auto d = doubleCls.safeGet(obj)) {
            return *d;
        }
        if (auto arr = safeGetArray(obj)) {
            return std::move(*arr);
        }
        if (auto o = getObject(obj)) {
            return std::move(*o);
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
