// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/dict.h>
#include "s-unity.h"

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
    ac_dict_root* root = ac_dict_new_root_from_json(json, NULL);
    CHECK_NOT_NULL(root);
    ac_dict_ref rr = ac_dict_make_ref(root);
    CHECK_NOT_NULL(rr);
    CHECK(ac_dict_get_type(rr) == ac_dict_value_type_object);
    CHECK(ac_dict_get_size(rr) == 6);


    ac_dict_ref key = ac_dict_at_key(rr, "key");
    CHECK_NOT_NULL(key);
    CHECK(ac_dict_get_type(key) == ac_dict_value_type_string);
    CHECK(ac_dict_get_size(key) == 1);
    const char* key_str = ac_dict_get_string_value(key);
    CHECK_EQ_STR("value", key_str);

    ac_dict_ref seven = ac_dict_at_key(rr, "seven");
    CHECK_NOT_NULL(seven);
    CHECK(ac_dict_get_type(seven) == ac_dict_value_type_number_unsigned);
    CHECK_EQ(7, ac_dict_get_int_value(seven));

    ac_dict_ref obj = ac_dict_at_key(rr, "obj");
    CHECK_NOT_NULL(obj);
    CHECK(ac_dict_get_type(obj) == ac_dict_value_type_object);
    CHECK(ac_dict_get_size(obj) == 2);

    ac_dict_ref nested = ac_dict_at_key(obj, "nested");
    CHECK_NOT_NULL(nested);
    CHECK(ac_dict_get_type(nested) == ac_dict_value_type_string);
    CHECK_EQ_STR("nv", ac_dict_get_string_value(nested));

    ac_dict_ref nested_bool = ac_dict_at_key(obj, "nested_bool");
    CHECK_NOT_NULL(nested_bool);
    CHECK(ac_dict_get_type(nested_bool) == ac_dict_value_type_bool);
    CHECK_EQ(true, ac_dict_get_bool_value(nested_bool));

    ac_dict_ref none = ac_dict_at_key(rr, "none");
    CHECK_NOT_NULL(none);
    CHECK(ac_dict_get_type(none) == ac_dict_value_type_null);
    CHECK(ac_dict_get_size(none) == 0);

    ac_dict_ref pi = ac_dict_at_key(rr, "pi");
    CHECK_NOT_NULL(pi);
    CHECK(ac_dict_get_type(pi) == ac_dict_value_type_number_double);
    CHECK_CLOSE(1e-5, 3.14159, ac_dict_get_double_value(pi));

    ac_dict_ref ar = ac_dict_at_key(rr, "ar");
    CHECK_NOT_NULL(ar);
    CHECK(ac_dict_get_type(ar) == ac_dict_value_type_array);
    CHECK(ac_dict_get_size(ar) == 3);

    ac_dict_ref ar0 = ac_dict_at_index(ar, 0);
    CHECK_NOT_NULL(ar0);
    CHECK(ac_dict_get_type(ar0) == ac_dict_value_type_number_unsigned);
    CHECK_EQ(1, ac_dict_get_int_value(ar0));

    ac_dict_ref ar1 = ac_dict_at_index(ar, 1);
    CHECK_NOT_NULL(ar1);
    CHECK(ac_dict_get_type(ar1) == ac_dict_value_type_number_int);
    CHECK_EQ(-2, ac_dict_get_int_value(ar1));

    ac_dict_ref ar2 = ac_dict_at_index(ar, 2);
    CHECK_NOT_NULL(ar2);
    CHECK(ac_dict_get_type(ar2) == ac_dict_value_type_string);
    CHECK_EQ_STR("three", ac_dict_get_string_value(ar2));

    // iteration
    ac_dict_free_iter(NULL); // should be safe

    const ac_dict_value_type expected_ar_types[] = {
        ac_dict_value_type_number_unsigned,
        ac_dict_value_type_number_int,
        ac_dict_value_type_string
    };

    int i = 0;
    for (ac_dict_iter* it = ac_dict_new_iter(ar); it; it = ac_dict_iter_next(it)) {
        const char* k = ac_dict_iter_get_key(it);
        ac_dict_ref v = ac_dict_iter_get_value(it);
        CHECK_NULL(k);
        CHECK_NOT_NULL(v);
        CHECK(ac_dict_get_type(v) == expected_ar_types[i++]);
    }

    ac_dict_free_root(root);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(parse);
    return UNITY_END();
}
