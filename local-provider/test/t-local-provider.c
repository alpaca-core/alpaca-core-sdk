// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "tl-dummy-local-provider.h"
#include <ac/dict.h>
#include <ac-test-util/unity.h>

#include <string.h>
#include <stdatomic.h>

void setUp(void) {}
void tearDown(void) {}

typedef struct state {
    ac_model* model;
    ac_instance* instance;
    ac_dict_root* dict_root;
    ac_dict_ref dict;
    char last_error[1024];
    float last_progress;
    atomic_bool cur_step_done;
    uint64_t main_thread_id;
} state;

void set_error(state* s, const char* error) {
    if (error) {
        size_t size = strlen(error) + 1;
        strncpy(s->last_error, error, size);
    }
    else {
        s->last_error[0] = 0;
    }
}

void on_progress(float progress, void* user_data) {
    state* s = (state*)user_data;
    CHECK_GT_FLT(s->last_progress, progress);
    s->last_progress = progress;
}

void on_model_result(ac_model* m, const char* error, void* user_data) {
    state* s = (state*)user_data;
    CHECK_NULL(s->model);
    s->model = m;
    set_error(s, error);

    TEST_ASSERT_NOT_EQUAL_UINT64(s->main_thread_id, get_thread_id());
    atomic_store(&s->cur_step_done, true);
}

void on_instance_result(ac_instance* i, const char* error, void* user_data) {
    state* s = (state*)user_data;
    CHECK_NULL(s->instance);
    s->instance = i;
    set_error(s, error);

    TEST_ASSERT_NOT_EQUAL_UINT64(s->main_thread_id, get_thread_id());
    atomic_store(&s->cur_step_done, true);
}

void on_op_result(const char* error, void* user_data) {
    state* s = (state*)user_data;
    s->last_progress = 0;
    set_error(s, error);

    TEST_ASSERT_NOT_EQUAL_UINT64(s->main_thread_id, get_thread_id());
    atomic_store(&s->cur_step_done, true);
}

void on_op_stream(ac_dict_ref dict, void* user_data) {
    state* s = (state*)user_data;
    if (s->dict_root) {
        ac_dict_free_root(s->dict_root);
    }
    s->dict_root = ac_dict_new_root();
    s->dict = ac_dict_make_ref(s->dict_root);
    ac_dict_take(s->dict, dict);

    TEST_ASSERT_NOT_EQUAL_UINT64(s->main_thread_id, get_thread_id());
}

void wait_for_cur_step(state* s) {
    // just spin lock
    // it's the easiest way and effective resource management is not a priority in this test
    while (!atomic_load(&s->cur_step_done)) {
        // spin
    }
    atomic_store(&s->cur_step_done, false); // prepare next step
}

void dummy_provider(void) {
    ac_api_provider* provider = create_dummy_provider();
    CHECK_NOT_NULL(provider);
    ac_add_local_inference(provider);

    state s = {0};
    s.main_thread_id = get_thread_id();
    atomic_init(&s.cur_step_done, false);

    {
        ac_create_model_json_params(provider, "{\"type\": \"llama.cpp\", \"error\": true}", NULL, on_model_result, on_progress, &s);
        wait_for_cur_step(&s);

        CHECK_EQ_STR("Model couldn't be loaded!", s.last_error);
        CHECK_EQ_FLT(0.5f, s.last_progress);
        CHECK_NULL(s.model);
        s.last_progress = 0;
    }

    {
        ac_create_model_json_params(provider, "{}", NULL, on_model_result, on_progress, &s);
        wait_for_cur_step(&s);

        CHECK_EQ_STR("[json.exception.out_of_range.403] key 'type' not found", s.last_error);
        CHECK_NULL(s.model);
    }

    {
        ac_create_model_json_params(provider, "{\"type\": \"llama.cpp\"}", NULL, on_model_result, on_progress, &s);
        wait_for_cur_step(&s);

        CHECK_EQ_STR("", s.last_error);
        CHECK_NOT_NULL(s.model);
    }

    {
        ac_create_instance_json_params(s.model, "error", "{\"error\": \"bad inst\"}", NULL, on_instance_result, on_progress, &s);
        wait_for_cur_step(&s);

        CHECK_EQ_STR("[json.exception.type_error.302] type must be boolean, but is string", s.last_error);
        CHECK_NULL(s.instance);
    }

    {
        ac_create_instance_json_params(s.model, "error", "{\"error\": true}", NULL, on_instance_result, on_progress, &s);
        wait_for_cur_step(&s);

        CHECK_EQ_STR("Instance couldn't be created!", s.last_error);
        CHECK_NULL(s.instance);
    }

    {
        ac_create_instance_json_params(s.model, "insta", "{}", NULL, on_instance_result, on_progress, &s);
        wait_for_cur_step(&s);

        CHECK_EQ_STR("", s.last_error);
        CHECK_NOT_NULL(s.instance);
    }

    {
        ac_run_op_json_params(s.instance, "op", "{}", NULL, on_op_result, on_op_stream, &s);
        ac_synchronize_instance(s.instance);

        ac_dict_ref some = ac_dict_at_key(s.dict, "some");
        CHECK_NOT_NULL(some);
        CHECK_EQ(ac_dict_value_type_number_int, ac_dict_get_type(some));
        CHECK_EQ(42, ac_dict_get_int_value(some));
    }

    {
        ac_run_op_json_params(s.instance, "error", "{\"error\": \"bad op\"}", NULL, on_op_result, on_op_stream, &s);
        ac_synchronize_instance(s.instance);

        ac_dict_ref error = ac_dict_at_key(s.dict, "error");
        CHECK_NOT_NULL(error);
        CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(error));
        CHECK_EQ_STR("bad op", ac_dict_get_string_value(error));
    }

    {
        ac_run_op_json_params(s.instance, "more", "{}", NULL, on_op_result, on_op_stream, &s);
        ac_synchronize_instance(s.instance);

        ac_dict_ref more = ac_dict_at_key(s.dict, "more");
        CHECK_NOT_NULL(more);
        CHECK_EQ(ac_dict_value_type_number_int, ac_dict_get_type(more));
        CHECK_EQ(1024, ac_dict_get_int_value(more));
    }

    {
        ac_run_op_json_params(s.instance, "insta", "{}", NULL, on_op_result, on_op_stream, &s);
        ac_synchronize_instance(s.instance);

        ac_dict_ref insta = ac_dict_at_key(s.dict, "insta");
        CHECK_NOT_NULL(insta);
        CHECK_EQ(ac_dict_value_type_string, ac_dict_get_type(insta));
        CHECK_EQ_STR("success", ac_dict_get_string_value(insta));
    }

    ac_dict_free_root(s.dict_root);
    ac_free_instance(s.instance);
    ac_free_model(s.model);
    ac_free_api_provider(provider);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(dummy_provider);
    return UNITY_END();
}
