// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "jni.hpp"
#include "JniDict.hpp"
#include <astl/throw_ex.hpp>

extern "C" {


JNIEXPORT jni::jobject* JNICALL
Java_com_alpacacore_TestDict_getObjectFromDictByJson(jni::JNIEnv* env, jni::jclass* /*cls*/, jni::jstring* jjson) {
    auto json = jni::Make<std::string>(*env, jni::Local<jni::String>(*env, jjson));
    if (json.empty()) {
        return ac::java::Dict_toObject(*env, {}).release();
    }
    auto dict = ac::Dict::parse(std::move(json));
    return ac::java::Dict_toObject(*env, dict).release();
}

JNIEXPORT jni::jobject* JNICALL
Java_com_alpacacore_TestDict_getObjectFromDictWithBinary(jni::JNIEnv* env, jni::jclass* /*cls*/) {
    ac::Dict dict;
    dict["int"] = 3;
    dict["str"] = "hello";

    ac::Blob blob;
    blob.reserve(256);
    for (int i = 0; i < 256; ++i) {
        blob.push_back(uint8_t(i));
    }
    dict["bytes"] = ac::Dict::binary(std::move(blob));
    return ac::java::Dict_toObject(*env, dict).release();
}

JNIEXPORT jboolean JNICALL
Java_com_alpacacore_TestDict_runCppTestWithNullObject(jni::JNIEnv* env, jni::jclass* /*cls*/, jni::jobject* obj) try {
    auto dict = ac::java::Object_toDict(*env, jni::Local<ac::java::Obj>(*env, obj));
    return dict.is_null();
}
catch (...) {
    jni::ThrowJavaError(*env, std::current_exception());
    return false; // reachable by C++, but not by Java
}

void do_test(bool b, const char* check, const char* file, int line) {
    if (!b) {
        ac::throw_ex{} << "FAIL " << file << ":" << line << ": " << check;
    }
}

#define TEST(b) do_test((b), #b, __FILE__, __LINE__)

JNIEXPORT jboolean JNICALL
Java_com_alpacacore_TestDict_runCppTestWithJsonLikeObject(JNIEnv* env, jni::jclass* /*cls*/, jni::jobject* javaObj) try {
    auto dict = ac::java::Object_toDict(*env, jni::Local<ac::java::Obj>(*env, javaObj));
    /*
    Map map = new HashMap();
    map.put("false", false);
    map.put("null", null);
    map.put("int", 101);
    map.put("long_i", 5L);
    map.put("long_u", 3000000000L);
    map.put("long_d", 5000000000L);
    map.put("long_d2", -3000000000L);
    map.put("str", "hello");

    Map obj1 = new HashMap();
    obj1.put("key", 1);
    obj1.put("pi", 3.14);
    obj1.put("empty_list", new Object[0]);
    obj1.put("full_list", new Object[]{ 1, "horse", false });

    map.put("obj", obj1);

    Map inAr = new HashMap();
    inAr.put("key", 1);
    inAr.put("key2", "val");
    Object[] arr = { inAr, true, 0.5, "world" };
    map.put("arr", arr);
    */

    TEST(dict.size() == 10);
    TEST(dict.is_object());
    TEST(dict["false"].is_boolean());
    TEST(dict["false"].get<bool>() == false);
    TEST(dict["null"].is_null());
    TEST(dict["int"].is_number_integer());
    TEST(dict["int"].get<int>() == 101);
    TEST(dict["long_i"].is_number_integer());
    TEST(dict["long_i"].get<int>() == 5);
    TEST(dict["long_u"].is_number_unsigned());
    TEST(dict["long_u"].get<unsigned>() == 3'000'000'000u);
    TEST(dict["long_d"].is_number_float());
    TEST(dict["long_d"].get<double>() == 5'000'000'000.0);
    TEST(dict["long_d2"].is_number_float());
    TEST(dict["long_d2"].get<double>() == -3'000'000'000.0);
    TEST(dict["str"].is_string());
    TEST(dict["str"].get<std::string_view>() == "hello");

    auto obj = dict["obj"];
    TEST(obj.is_object());
    TEST(obj.size() == 4);
    TEST(obj["key"].is_number_integer());
    TEST(obj["key"].get<int>() == 1);
    TEST(obj["pi"].is_number_float());
    TEST(obj["pi"].get<double>() == 3.14);
    TEST(obj["empty_list"].is_array());
    TEST(obj["empty_list"].size() == 0);

    auto full_list = obj["full_list"];
    TEST(full_list.is_array());
    TEST(full_list.size() == 3);
    TEST(full_list[0].is_number_integer());
    TEST(full_list[0].get<int>() == 1);
    TEST(full_list[1].is_string());
    TEST(full_list[1].get<std::string_view>() == "horse");
    TEST(full_list[2].is_boolean());
    TEST(full_list[2].get<bool>() == false);

    auto arr = dict["arr"];
    TEST(arr.is_array());
    TEST(arr.size() == 4);

    auto inAr = arr[0];
    TEST(inAr.is_object());
    TEST(inAr.size() == 2);
    TEST(inAr["key"].is_number_integer());
    TEST(inAr["key"].get<int>() == 1);
    TEST(inAr["key2"].is_string());
    TEST(inAr["key2"].get<std::string_view>() == "val");

    TEST(arr[1].is_boolean());
    TEST(arr[1].get<bool>() == true);
    TEST(arr[2].is_number_float());
    TEST(arr[2].get<double>() == 0.5);
    TEST(arr[3].is_string());
    TEST(arr[3].get<std::string_view>() == "world");

    return true;
}
catch (...) {
    jni::ThrowJavaError(*env, std::current_exception());
    return false; // reachable by C++, but not by Java
}

JNIEXPORT jboolean JNICALL
Java_com_alpacacore_TestDict_runCppTestWithObjectWithBinary(JNIEnv* env, jni::jclass* /*cls*/, jni::jobject* javaObj) try {
    auto dict = ac::java::Object_toDict(*env, jni::Local<ac::java::Obj>(*env, javaObj));

    /*
    Map map = new HashMap();
    map.put("hello", "wold");

    byte[] bytes = new byte[256];
    for (int i = 0; i < 256; i++) {
        bytes[i] = (byte)(255 - i);
    }

    map.put("bytes", bytes);
    */

    TEST(dict.size() == 2);
    TEST(dict["hello"].is_string());
    TEST(dict["hello"].get<std::string_view>() == "wold");
    TEST(dict["bytes"].is_binary());

    auto& blob = dict["bytes"].get_binary();
    TEST(blob.size() == 256);
    for (int i = 0; i < 256; ++i) {
        TEST(blob[i] == uint8_t(255 - i));
    }

    return true;
}
catch (...) {
    jni::ThrowJavaError(*env, std::current_exception());
    return false; // reachable by C++, but not by Java
}


} // extern "C"
