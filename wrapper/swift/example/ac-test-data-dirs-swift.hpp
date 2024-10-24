// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

#include "../../../gen/ac-test-data-whisper-dir.h"
#include "../../../gen/ac-test-data-llama-dir.h"
#include "../../../gen/ac-test-data-dummy-dir.h"

namespace AC {

const char* getWhisperDir() {
    return AC_TEST_DATA_WHISPER_DIR;
}

const char* getLlamaDir() {
    return AC_TEST_DATA_LLAMA_DIR;
}

const char* getDummyDir() {
    return AC_TEST_DATA_DUMMY_DIR;
}

}
