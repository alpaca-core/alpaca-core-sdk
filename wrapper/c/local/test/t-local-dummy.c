// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local.h>
#include <ac/local_dummy.h>
#include <ac/dict.h>
#include <ac-unity.h>

#include "ac-test-data-dummy-models.h"

void free_null(void) {
    // all must be safe
    ac_free_local_model(NULL);
    ac_free_local_instance(NULL);
    ac_free_local_model_factory(NULL);
}

typedef struct progress_info {
    char tag[1024];
    float progress;
} progress_info;

bool on_progress(ac_sv tag, float progress, void* u) {
    if (!u) return true;
    progress_info* info = (progress_info*)u;
    size_t len = ac_sv_len(tag);
    CHECK(len - 1 < sizeof(info->tag));
    memcpy(info->tag, tag.begin, len);
    info->tag[len] = '\0';
    info->progress = progress;
    return true;
}

ac_dict_arg make_params(ac_dict_ref temp, const char* json) {
    ac_dict_parse_json(temp, json, NULL);
    return ac_dict_arg_take(temp);
}

void local_dummy(void) {
    ac_local_model_factory* factory = ac_new_local_model_factory();

    CHECK_NOT_NULL(factory);
    ac_local_model* model = ac_create_local_model(factory, "dummy", NULL, 0, ac_dict_arg_null(), NULL, NULL);
    CHECK_NULL(model);

    CHECK_EQ_STR("Unknown inference type: dummy", ac_local_get_last_error());

    ac_add_local_dummy_inference(factory);

    ac_local_model_desc_asset model_asset = {
        .path = AC_DUMMY_MODEL_SMALL,
        .tag = "x"
    };
    progress_info info = {0};
    model = ac_create_local_model(factory, "dummy", &model_asset, 1, ac_dict_arg_null(), on_progress, &info);
    CHECK_NULL(ac_local_get_last_error());
    CHECK_NOT_NULL(model);
    CHECK_EQ_STR(AC_DUMMY_MODEL_SMALL, info.tag);
    CHECK_GT_FLT(0.0f, info.progress);

    ac_local_instance* instance = ac_create_local_instance(model, "nope", ac_dict_arg_null());
    CHECK_NULL(instance);
    CHECK_EQ_STR("dummy: unknown instance type: nope", ac_local_get_last_error());

    ac_dict_root* params_root = ac_dict_new_root();
    ac_dict_ref params = ac_dict_make_ref(params_root);

    instance = ac_create_local_instance(model, "general", make_params(params, "{\"cutoff\": 30}"));
    CHECK_NULL(instance);
    CHECK_EQ_STR("Cutoff 30 greater than model size 3", ac_local_get_last_error());

    instance = ac_create_local_instance(model, "general", ac_dict_arg_null());
    CHECK_NOT_NULL(instance);
    CHECK_NULL(ac_local_get_last_error());

    ac_dict_root* result_root = ac_dict_new_root();
    ac_dict_ref result = ac_dict_make_ref(result_root);

    result = ac_run_local_op(ac_dict_make_ref(result_root), instance, "nope", ac_dict_arg_null(), NULL, NULL);
    CHECK_NULL(result);
    CHECK_EQ_STR("dummy: unknown op: nope", ac_local_get_last_error());

    result = ac_run_local_op(ac_dict_make_ref(result_root), instance, "run",
        make_params(params, "{\"input\": [\"a\", \"b\"]}"), NULL, NULL);
    CHECK_NOT_NULL(result);
    CHECK_NULL(ac_local_get_last_error());
    CHECK_EQ_STR("a soco b bate", ac_dict_get_string_value(ac_dict_at_key(result, "result")));


    result = ac_run_local_op(ac_dict_make_ref(result_root), instance, "run",
        make_params(params, "{\"input\": [\"a\", \"b\"], \"splice\": false}"), NULL, NULL);
    CHECK_NOT_NULL(result);
    CHECK_NULL(ac_local_get_last_error());
    CHECK_EQ_STR("a b soco bate vira", ac_dict_get_string_value(ac_dict_at_key(result, "result")));


    result = ac_run_local_op(ac_dict_make_ref(result_root), instance, "run",
        make_params(params, "{\"input\": [\"a\", \"b\"], \"throw_on\": 2}"), NULL, NULL);
    CHECK_NULL(result);
    CHECK_EQ_STR("Throw on token 2", ac_local_get_last_error());

    ac_local_instance* cutoff_instance = ac_create_local_instance(model, "general",
        make_params(params, "{\"cutoff\": 2}"));
    CHECK_NOT_NULL(cutoff_instance);
    CHECK_NULL(ac_local_get_last_error());

    result = ac_run_local_op(ac_dict_make_ref(result_root), cutoff_instance, "run",
        make_params(params, "{\"input\": [\"a\", \"b\", \"c\"]}"), NULL, NULL);
    CHECK_NOT_NULL(result);
    CHECK_NULL(ac_local_get_last_error());
    CHECK_EQ_STR("a soco b bate c soco", ac_dict_get_string_value(ac_dict_at_key(result, "result")));


    ac_local_model* synthetic_model = ac_create_local_model(factory,
        "dummy", NULL, 0, ac_dict_arg_null(), on_progress, &info);
    CHECK_NOT_NULL(synthetic_model);
    CHECK_NULL(ac_local_get_last_error());
    CHECK_EQ_STR("synthetic", info.tag);
    CHECK_EQ_FLT(0.5f, info.progress);

    ac_local_instance* synthetic_instance = ac_create_local_instance(synthetic_model, "general", ac_dict_arg_null());
    CHECK_NOT_NULL(synthetic_instance);
    CHECK_NULL(ac_local_get_last_error());

    result = ac_run_local_op(ac_dict_make_ref(result_root), synthetic_instance, "run",
        make_params(params, "{\"input\": [\"x\", \"y\"]}"), NULL, NULL);
    CHECK_NOT_NULL(result);
    CHECK_NULL(ac_local_get_last_error());
    CHECK_EQ_STR("x one y two", ac_dict_get_string_value(ac_dict_at_key(result, "result")));


    ac_dict_free_root(params_root);
    ac_dict_free_root(result_root);
    ac_free_local_instance(synthetic_instance);
    ac_free_local_model(synthetic_model);
    ac_free_local_instance(cutoff_instance);
    ac_free_local_instance(instance);
    ac_free_local_model(model);
    ac_free_local_model_factory(factory);
}

void setUp(void) {}
void tearDown(void) {}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(free_null);
    RUN_TEST(local_dummy);
    //RUN_TEST(dummy_model);
    return UNITY_END();
}