// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/api.h>
#include <ac/dict.h>
#include <ac-test-util/unity.h>
#include <string.h>

ac_model* create_dummy_model(void);
void setUp(void) {}
void tearDown(void) {}

void free_null(void) {
    // all must be safe
    ac_free_model(NULL);
    ac_free_instance(NULL);
}

typedef struct state {
    ac_model* model;
    ac_instance* instance;
    ac_dict_root* dict_root;
    ac_dict_ref dict;
    char last_error[1024];
    float last_progress;
} state;

void set_error(state* s, const char* error) {
    if (error) {
        strncpy(s->last_error, error, sizeof(s->last_error));
    }
    else {
        s->last_error[0] = 0;
    }
}

void on_progress(ac_sv tag, float progress, void* user_data) {
    CHECK_FALSE(ac_sv_is_empty(tag));
    state* s = (state*)user_data;
    CHECK_GT_FLT(s->last_progress, progress);
    if (progress == 1) {
        s->last_progress = 0;
    }
    else {
        s->last_progress = progress;
    }
}

void on_model_result(ac_model* m, const char* error, void* user_data) {
    state* s = (state*)user_data;
    CHECK_NULL(s->model);
    s->model = m;
    set_error(s, error);
}

void on_instance_result(ac_instance* i, const char* error, void* user_data) {
    state* s = (state*)user_data;
    CHECK_NULL(s->instance);
    s->instance = i;
    set_error(s, error);
}

void on_op_result(const char* error, void* user_data) {
    state* s = (state*)user_data;
    s->last_progress = 0;
    set_error(s, error);
}

void on_op_stream(ac_dict_ref dict, void* user_data) {
    state* s = (state*)user_data;
    if (s->dict_root) {
        ac_dict_free_root(s->dict_root);
    }
    s->dict_root = ac_dict_new_root();
    s->dict = ac_dict_make_ref(s->dict_root);
    ac_dict_take(s->dict, dict);
}

void dummy_model(void) {
    state s = {0};
    s.model = create_dummy_model();
    CHECK_EQ_STR("", s.last_error);
    CHECK_NOT_NULL(s.model);

    ac_create_instance(
        s.model, "error",
        ac_dict_new_root_from_json("{\"error\": \"bad inst\"}", NULL),
        on_instance_result, &s
    );
    CHECK_EQ_STR("bad inst", s.last_error);
    CHECK_NULL(s.instance);
    s.last_progress = 0;

    ac_create_instance(
        s.model, "insta",
        NULL,
        on_instance_result, &s
    );
    CHECK_EQ_STR("", s.last_error);
    CHECK_NOT_NULL(s.instance);

    ac_run_op(
        s.instance, "error",
        ac_dict_new_root_from_json("{\"error\": \"bad op\"}", NULL),
        on_op_result, on_op_stream, on_progress, &s
    );
    CHECK_EQ_STR("bad op", s.last_error);
    CHECK_EQ_FLT(0, s.last_progress);
    ac_dict_ref some = ac_dict_at_key(s.dict, "some");
    CHECK_NOT_NULL(some);
    CHECK_EQ(ac_dict_value_type_number_int, ac_dict_get_type(some));
    CHECK_EQ(42, ac_dict_get_int_value(some));

    ac_run_op(
        s.instance, "op",
        ac_dict_new_root_from_json("{}", NULL),
        on_op_result, on_op_stream, on_progress, &s
    );
    CHECK_EQ_STR("", s.last_error);
    CHECK_EQ_FLT(0, s.last_progress);
    ac_dict_ref more = ac_dict_at_key(s.dict, "more");
    CHECK_NOT_NULL(more);
    CHECK_EQ(ac_dict_value_type_number_int, ac_dict_get_type(more));
    CHECK_EQ(1024, ac_dict_get_int_value(more));

    // expect the same behavior with param
    // dict_root = ac_dict_new_root_from_json("{}", NULL)
    // AND
    // dict_root = NULL
    ac_run_op(
        s.instance, "op",
        NULL,
        on_op_result, on_op_stream, on_progress, &s
    );
    CHECK_EQ_STR("", s.last_error);
    CHECK_EQ_FLT(0, s.last_progress);
    more = ac_dict_at_key(s.dict, "more");
    CHECK_NOT_NULL(more);
    CHECK_EQ(ac_dict_value_type_number_int, ac_dict_get_type(more));
    CHECK_EQ(1024, ac_dict_get_int_value(more));

    ac_run_op(
        s.instance, "insta",
        ac_dict_new_root_from_json("{}", NULL),
        on_op_result, on_op_stream, on_progress, &s
    );
    CHECK_EQ_STR("", s.last_error);
    CHECK_EQ_FLT(0, s.last_progress);
    ac_dict_ref insta = ac_dict_at_key(s.dict, "insta");
    CHECK_NOT_NULL(insta);
    CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(insta));
    CHECK_EQ_STR("success", ac_dict_get_string_value(insta));

    ac_dict_free_root(s.dict_root);
    ac_free_instance(s.instance);
    ac_free_model(s.model);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(free_null);
    RUN_TEST(dummy_model);
    return UNITY_END();
}
