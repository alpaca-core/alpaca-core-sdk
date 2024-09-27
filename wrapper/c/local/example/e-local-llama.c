// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local.h>
#include <ac/local_llama.h>
#include <ac/dict.h>

#include <jalogc.h>

#include <ac/unused.h>

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "ac-test-data-llama-dir.h"

bool on_progress(ac_sv tag, float progress, void* user_data) {
    UNUSED(user_data);
    printf(PRacsv " progress: %f\n", AC_PRINTF_SV(tag), progress);
    return true;
}


int main(void) {
    int ret = 0;
    jalogc_init((jalogc_init_params) {.add_default_sink = true});

    ac_local_model_factory* factory = ac_new_local_model_factory();
    ac_add_local_llama_inference(factory);

    ac_local_instance* instance = NULL;

    ac_local_model_desc_asset llama_gguf = {
        .path = AC_TEST_DATA_LLAMA_DIR "/gpt2-117m-q6_k.gguf",
    };

    printf("Loading model...\n");
    ac_local_model* model = ac_create_local_model(
        factory,
        "llama.cpp", &llama_gguf, 1,
        NULL, on_progress, NULL
    );
    if (!model) {
        ret = 1;
        goto cleanup;
    }

    printf("Creating instance...\n");
    instance = ac_create_local_instance(model, "general", NULL);
    if (!instance) {
        ret = 1;
        goto cleanup;
    }

#define PROMPT "He was slow to"
    printf("Running op. Prompt: %s\n", PROMPT);
    printf("Generation: ");
    ac_dict_root* result = ac_run_local_op(instance, "run",
        ac_dict_new_root_from_json("{\"prompt\": \"" PROMPT "\", \"max_tokens\": 30}", NULL),
        NULL, NULL
    );
    if (!result) {
        ret = 1;
        goto cleanup;
    }

    ac_dict_ref dict_ref = ac_dict_make_ref(result);
    ac_dict_ref result_val = ac_dict_at_key(dict_ref, "result");
    if (result_val) {
        printf("%s\n", ac_dict_get_string_value(result_val));
    }
    else {
        char* dump = ac_dict_dump(dict_ref, 2);
        fprintf(stderr, "Error bad dict:\n%s\n", dump);
        free(dump);
        ret = 1;
    }
    ac_dict_free_root(result);

cleanup:
    ac_free_local_instance(instance);
    ac_free_local_model(model);
    ac_free_local_model_factory(factory);
    return ret;
}
