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

template <class OutTagType, class T>
jni::Local<jni::Object<OutTagType>> unsafeCast(jni::JNIEnv& env, const T& object)
{
    auto ptr = reinterpret_cast<typename jni::Object<OutTagType>::UntaggedType*>(jni::NewLocal(env, object).release());
    return jni::Local<jni::Object<OutTagType>>(env, ptr);
}


struct DictToMapConverter {
    jni::JNIEnv& env;

    HashMapClass hashMapCls;

    DictToMapConverter(jni::JNIEnv& env)
        : env(env)
        , hashMapCls(env)
    {}

    jni::Local<Obj> convert(const Dict& dict) {
        switch (dict.type()) {
        case Dict::value_t::boolean:
            return jni::Box(env, jboolean(dict.get<bool>()));
        case Dict::value_t::number_integer:
            return jni::Box(env, dict.get<jint>());
        case Dict::value_t::number_unsigned: {
            auto uval = dict.get<unsigned>();
            if (uval <= unsigned(std::numeric_limits<jint>::max())) {
                // return int if it fits
                return jni::Box(env, jint(uval));
            }
            return jni::Box(env, jlong(uval));
        }
        case Dict::value_t::number_float:
            return jni::Box(env, dict.get<jdouble>());
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
        case Dict::value_t::binary: {
            auto& buf = dict.get_binary();
            auto ret = jni::Local<jni::Array<jbyte>>::New(env, buf.size());
            jni::SetArrayRegion(env, *ret.get(), 0, buf.size(), reinterpret_cast<const jni::jbyte*>(buf.data()));
            return ret;
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
    jni::Local<jni::Class<jni::ArrayTag<jbyte>>> byteArrClass;

    MapClass mapCls;

    MapToDictConverter(jni::JNIEnv& env)
        : env(env)
        , stringClass(jni::Class<jni::StringTag>::Find(env))
        , objArrayClass(jni::Class<jni::ArrayTag<Obj>>::Find(env))
        , byteArrClass(jni::Class<jni::ArrayTag<jbyte>>::Find(env))
        , mapCls(env)
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

    std::optional<Dict> safeGetObject(jni::Local<Obj>& obj) {
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

    std::optional<Dict> safeGetBinary(const jni::Local<Obj>& obj) {
        if (!obj.IsInstanceOf(env, byteArrClass)) {
            return {};
        }

        auto cast = jni::Cast<jni::ArrayTag<jbyte>>(env, byteArrClass, obj);
        jni::Local<jni::Array<jbyte>> arr(env, cast.release()); // silly secondary cast since Array is not Object :(

        auto size = arr.Length(env);

        std::vector<uint8_t> ret(size);

        jni::GetArrayRegion(env, *arr.get(), 0, size, reinterpret_cast<jbyte*>(ret.data()));

        return ret;
    }

    template <typename Tag, typename CppType>
    struct SafeUnboxer {
        static std::optional<CppType> safeGet(jni::JNIEnv& env, const jni::Local<Obj>& obj) {
            if (!obj.IsInstanceOf(env, jni::Class<Tag>::Singleton(env))) {
                return {};
            }

            auto cast = unsafeCast<Tag>(env, obj);
            return jni::Unbox(env, cast);
        }
    };

    using BoolClass = SafeUnboxer<jni::BooleanTag, jni::jboolean>;
    using IntClass = SafeUnboxer<jni::IntegerTag, jni::jint>;
    using LongClass = SafeUnboxer<jni::LongTag, jni::jlong>;
    using DoubleClass = SafeUnboxer<jni::DoubleTag, jni::jdouble>;

    Dict convert(jni::Local<Obj> obj) {
        if (getNull(obj)) {
            return {};
        }
        if (auto str = safeGetString(obj)) {
            return std::move(*str);
        }
        if (auto b = BoolClass::safeGet(env, obj)) {
            return !!*b;
        }
        if (auto i = IntClass::safeGet(env, obj)) {
            return *i;
        }
        if (auto l = LongClass::safeGet(env, obj)) {
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
        if (auto d = DoubleClass::safeGet(env, obj)) {
            return *d;
        }
        if (auto arr = safeGetArray(obj)) {
            return std::move(*arr);
        }
        if (auto o = safeGetObject(obj)) {
            return std::move(*o);
        }
        if (auto bin = safeGetBinary(obj)) {
            return Dict::binary(std::move(*bin));
        }

        throw std::runtime_error("Unsupported value type");
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
