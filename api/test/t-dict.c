// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/dict.h>
#include "s-unity.h"
#include <string.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void parse(void) {
    const char* json = "{"
        "\"key\": \"value\","
        "\"seven\" : 7,"
        "\"obj\" : {"
            "\"nested\": \"nv\","
            "\"nested_bool\" : true"
        "},"
        "\"none\" : null,"
        "\"pi\" : 3.14159,"
        "\"ar\" : [1, -2, \"three\"]"
    "}";
    ac_dict_root* root = ac_dict_new_root();
    CHECK_NOT_NULL(root);
    ac_dict_ref rr = ac_dict_make_ref(root);
    CHECK_NOT_NULL(rr);

    CHECK_TRUE(ac_dict_parse_json(rr, json, NULL));
    CHECK_NULL(ac_dict_get_last_error());

    CHECK_EQ(ac_dict_value_type_object, ac_dict_get_type(rr));
    const int rr_size = ac_dict_get_size(rr);
    CHECK_EQ(6, rr_size);

    ac_dict_ref key = ac_dict_at_key(rr, "key");
    CHECK_NOT_NULL(key);
    CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(key));
    CHECK_EQ(1, ac_dict_get_size(key));
    const char* key_str = ac_dict_get_string_value(key);
    CHECK_EQ_STR("value", key_str);

    ac_dict_ref seven = ac_dict_at_key(rr, "seven");
    CHECK_NOT_NULL(seven);
    CHECK_EQ(ac_dict_value_type_number_unsigned, ac_dict_get_type(seven));
    CHECK_EQ(7, ac_dict_get_int_value(seven));

    ac_dict_ref obj = ac_dict_at_key(rr, "obj");
    CHECK_NOT_NULL(obj);
    CHECK_EQ(ac_dict_value_type_object, ac_dict_get_type(obj));
    CHECK_EQ(2, ac_dict_get_size(obj));

    ac_dict_ref nested = ac_dict_at_key(obj, "nested");
    CHECK_NOT_NULL(nested);
    CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(nested));
    CHECK_EQ_STR("nv", ac_dict_get_string_value(nested));

    ac_dict_ref nested_bool = ac_dict_at_key(obj, "nested_bool");
    CHECK_NOT_NULL(nested_bool);
    CHECK_EQ(ac_dict_value_type_bool, ac_dict_get_type(nested_bool));
    CHECK_EQ(true, ac_dict_get_bool_value(nested_bool));

    ac_dict_ref none = ac_dict_at_key(rr, "none");
    CHECK_NOT_NULL(none);
    CHECK_EQ(ac_dict_value_type_null, ac_dict_get_type(none));
    CHECK_EQ(0, ac_dict_get_size(none));

    ac_dict_ref pi = ac_dict_at_key(rr, "pi");
    CHECK_NOT_NULL(pi);
    CHECK_EQ(ac_dict_value_type_number_double, ac_dict_get_type(pi));
    CHECK_CLOSE(1e-5, 3.14159, ac_dict_get_double_value(pi));

    ac_dict_ref ar = ac_dict_at_key(rr, "ar");
    CHECK_NOT_NULL(ar);
    CHECK_EQ(ac_dict_value_type_array, ac_dict_get_type(ar));
    CHECK_EQ(3, ac_dict_get_size(ar));

    ac_dict_ref ar_0 = ac_dict_at_index(ar, 0);
    CHECK_NOT_NULL(ar_0);
    CHECK_EQ(ac_dict_value_type_number_unsigned, ac_dict_get_type(ar_0));
    CHECK_EQ(1, ac_dict_get_int_value(ar_0));

    ac_dict_ref ar_1 = ac_dict_at_index(ar, 1);
    CHECK_NOT_NULL(ar_1);
    CHECK_EQ(ac_dict_value_type_number_int, ac_dict_get_type(ar_1));
    CHECK_EQ(-2, ac_dict_get_int_value(ar_1));

    ac_dict_ref ar_2 = ac_dict_at_index(ar, 2);
    CHECK_NOT_NULL(ar_2);
    CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(ar_2));
    CHECK_EQ_STR("three", ac_dict_get_string_value(ar_2));

    // iteration
    ac_dict_free_iter(NULL); // should be safe

    // object

    // order in object is alphabetical
    const char* expected_rr_keys[] = { "ar", "key", "none", "obj", "pi", "seven" };
    const ac_dict_ref expected_rr_values[] = { ar, key, none, obj, pi, seven };

    int i = 0;
    for (ac_dict_iter* it = ac_dict_new_iter(rr); it; it = ac_dict_iter_next(it)) {
        const char* k = ac_dict_iter_get_key(it);
        CHECK_EQ_STR(expected_rr_keys[i], k);
        ac_dict_ref v = ac_dict_iter_get_value(it);
        CHECK_EQ_PTR(expected_rr_values[i], v);
        ++i;
    }

    // array
    const ac_dict_value_type expected_ar_types[] = {
        ac_dict_value_type_number_unsigned,
        ac_dict_value_type_number_int,
        ac_dict_value_type_string
    };

    i = 0;
    for (ac_dict_iter* it = ac_dict_new_iter(ar); it; it = ac_dict_iter_next(it)) {
        const char* k = ac_dict_iter_get_key(it);
        ac_dict_ref v = ac_dict_iter_get_value(it);
        CHECK_NULL(k);
        CHECK_NOT_NULL(v);
        CHECK_EQ(expected_ar_types[i++], ac_dict_get_type(v));
    }

    // break iteration (should not leak)
    i = 0;
    for (ac_dict_iter* it = ac_dict_new_iter(rr); it; it = ac_dict_iter_next(it)) {
        if (i == 1) {
            ac_dict_free_iter(it);
            break;
        }
    }

    // errors
    CHECK_NULL(ac_dict_at_key(rr, "miss"));
    {
        const char* error = ac_dict_get_last_error();
        CHECK_NOT_NULL(error);
        CHECK_NOT_NULL(strstr(error, "'miss'"));
        CHECK_NOT_NULL(strstr(error, "not found"));
    }

    ar_1 = ac_dict_at_index(ar, 1); // make successful call
    CHECK_NULL(ac_dict_get_last_error()); // error must be cleared

    CHECK_FALSE(ac_dict_parse_json(key, "not json", NULL));
    CHECK_NOT_NULL(ac_dict_get_last_error());
    // then key is unchanged
    CHECK_NOT_NULL(key);
    CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(key));
    CHECK_EQ_STR(key_str, ac_dict_get_string_value(key));

    int ei = ac_dict_get_int_value(key);
    CHECK_EQ(0, ei);
    CHECK_NOT_NULL(ac_dict_get_last_error());

    ac_dict_free_root(root);
}

void build(void) {
    ac_dict_root* root = ac_dict_new_root();
    CHECK_NOT_NULL(root);
    ac_dict_ref rr = ac_dict_make_ref(root);
    CHECK_EQ(ac_dict_value_type_null, ac_dict_get_type(rr));

    ac_dict_set_int(rr, 7);
    CHECK_EQ(ac_dict_value_type_number_int, ac_dict_get_type(rr));
    CHECK_EQ(7, ac_dict_get_int_value(rr));

    ac_dict_set_object(rr); // change should be valid
    CHECK_EQ(ac_dict_value_type_object, ac_dict_get_type(rr));
    CHECK_EQ(0, ac_dict_get_size(rr));

    ac_dict_ref i = ac_dict_add_child(rr, "i");
    CHECK_NOT_NULL(i);
    CHECK_EQ(ac_dict_value_type_null, ac_dict_get_type(i));
    ac_dict_set_int(i, 42);
    CHECK_EQ(ac_dict_value_type_number_int, ac_dict_get_type(i));
    CHECK_EQ(42, ac_dict_get_int_value(i));

    ac_dict_ref str = ac_dict_add_child(rr, "str");
    CHECK_NOT_NULL(str);
    ac_dict_set_string(str, "hello", NULL);
    CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(str));
    CHECK_EQ_STR("hello", ac_dict_get_string_value(str));

    ac_dict_ref ar = ac_dict_add_child(rr, "ar");
    CHECK_NOT_NULL(ar);
    ac_dict_set_array(ar);
    CHECK_EQ(ac_dict_value_type_array, ac_dict_get_type(ar));
    CHECK_EQ(0, ac_dict_get_size(ar));
    ac_dict_ref ar_0 = ac_dict_add_child(ar, NULL);
    CHECK_NOT_NULL(ar_0);
    ac_dict_set_double(ar_0, 3.14);
    CHECK_EQ(ac_dict_value_type_number_double, ac_dict_get_type(ar_0));
    CHECK_CLOSE(1e-5, 3.14, ac_dict_get_double_value(ar_0));
    ac_dict_ref ar_1 = ac_dict_add_child(ar, NULL);
    CHECK_NOT_NULL(ar_1);
    ac_dict_set_bool(ar_1, true);
    CHECK_EQ(ac_dict_value_type_bool, ac_dict_get_type(ar_1));
    CHECK_EQ(true, ac_dict_get_bool_value(ar_1));
    ac_dict_ref ar_2 = ac_dict_add_child(ar, NULL);
    CHECK_NOT_NULL(ar_2);
    ac_dict_copy(ar_2, str);
    CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(ar_2));
    CHECK_EQ_STR("hello", ac_dict_get_string_value(ar_2));
    CHECK(ac_dict_get_size(ar) == 3);

    char* d1 = ac_dict_dump(rr, 2);
    char d2[1024];
    ac_dict_dump_to(rr, 2, d2, sizeof(d2));
    CHECK_EQ_STR(d1, d2);

    ac_dict_root* root2 = ac_dict_new_root();
    ac_dict_ref rr2 = ac_dict_make_ref(root2);
    ac_dict_take(rr2, rr);
    CHECK_EQ(ac_dict_value_type_object, ac_dict_get_type(rr2));
    CHECK_EQ(ac_dict_value_type_null, ac_dict_get_type(rr));

    // children should be preserved
    CHECK_EQ(str, ac_dict_at_key(rr2, "str"));
    ac_dict_ref ar2 = ac_dict_at_key(rr2, "ar");
    CHECK_EQ(ar, ar2);
    CHECK_EQ(ar_2, ac_dict_at_index(ar2, 2));

    free(d1);
    ac_dict_free_root(root);
    ac_dict_free_root(root2);
}

void binary(void) {
    ac_dict_root* root = ac_dict_new_root();
    ac_dict_ref rr = ac_dict_make_ref(root);
    ac_dict_set_object(rr);
    ac_dict_ref str = ac_dict_add_child(rr, "str");
    ac_dict_set_string(str, "hello", NULL);

    ac_dict_ref blob = ac_dict_add_child(rr, "blob");

    uint8_t buf[100];
    for (int i = 0; i < sizeof(buf); ++i) {
        buf[i] = (uint8_t)(i + 5);
    }

    ac_dict_set_binary(blob, buf, sizeof(buf));
    CHECK_EQ(ac_dict_value_type_binary, ac_dict_get_type(blob));
    ac_dict_binary_buf b = ac_dict_get_binary_value(blob);
    CHECK_EQ(sizeof(buf), b.size);
    CHECK_MEM_EQ(buf, b.data, sizeof(buf));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(parse);
    RUN_TEST(build);
    RUN_TEST(binary);
    return UNITY_END();
}
