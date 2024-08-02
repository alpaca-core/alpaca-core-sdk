// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

// include unity
// and add shorthand macros

#include <unity.h>

#define CHECK       TEST_ASSERT
#define CHECK_FALSE TEST_ASSERT_FALSE

#define CHECK_NULL      TEST_ASSERT_NULL
#define CHECK_NOT_NULL  TEST_ASSERT_NOT_NULL
#define CHECK_EQ        TEST_ASSERT_EQUAL
#define CHECK_EQ_PTR    TEST_ASSERT_EQUAL_PTR
#define CHECK_EQ_STR    TEST_ASSERT_EQUAL_STRING
#define CHECK_EQ_FLT    TEST_ASSERT_EQUAL_FLOAT
#define CHECK_TRUE      TEST_ASSERT_TRUE
#define CHECK_FALSE     TEST_ASSERT_FALSE
#define CHECK_GT        TEST_ASSERT_GREATER_THAN
#define CHECK_GTE       TEST_ASSERT_GREATER_OR_EQUAL
#define CHECK_GT_FLT    TEST_ASSERT_GREATER_THAN_FLOAT
#define CHECK_CLOSE     TEST_ASSERT_FLOAT_WITHIN

#define CHECK_SUCCESS(x) CHECK_EQ(0, x)
#define CHECK_FAIL(x)    TEST_ASSERT_NOT_EQUAL(0, x)

#define CHECK_MEM_EQ TEST_ASSERT_EQUAL_MEMORY

#define CHECK_AR_ALL_PTR TEST_ASSERT_EACH_EQUAL_PTR

#include <splat/warnings.h>
DISABLE_GCC_AND_CLANG_WARNING("-Wmissing-field-initializers")
