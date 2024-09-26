// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "t-sv.h"
#include <ac/sv.h>
#include <ac-unity.h>

static void sv(void) {
    ac_sv e = { 0 };
    CHECK_NULL(e.begin);
    CHECK_NULL(e.end);
    CHECK(ac_sv_is_null(e));
    CHECK(ac_sv_is_empty(e));
    CHECK(ac_sv_len(e) == 0);
    CHECK(ac_sv_cmp(e, e) == 0);
    CHECK(ac_sv_starts_with(e, ""));
    CHECK_FALSE(ac_sv_starts_with(e, "x"));
    CHECK_NULL(ac_sv_find_first(e, 'x'));
    CHECK_NULL(ac_sv_find_last(e, 'x'));

    ac_sv e2 = ac_make_sv_str("");
    CHECK_NOT_NULL(e2.begin);
    CHECK_NOT_NULL(e2.end);
    CHECK_FALSE(ac_sv_is_null(e2));
    CHECK(ac_sv_is_empty(e2));
    CHECK(ac_sv_len(e2) == 0);
    CHECK(ac_sv_cmp(e2, e2) == 0);
    CHECK(ac_sv_cmp(e, e2) == 0);
    CHECK(ac_sv_cmp(e2, e) == 0);
    CHECK(ac_sv_starts_with(e2, ""));
    CHECK_FALSE(ac_sv_starts_with(e2, "x"));
    CHECK_NULL(ac_sv_find_first(e2, 'x'));
    CHECK_NULL(ac_sv_find_last(e2, 'x'));

    {
        ac_sv abc = ac_make_sv_str("abc");
        CHECK_NOT_NULL(abc.begin);
        CHECK_NOT_NULL(abc.end);
        CHECK_FALSE(ac_sv_is_null(abc));
        CHECK_FALSE(ac_sv_is_empty(abc));
        CHECK(ac_sv_len(abc) == 3);
        CHECK(ac_sv_cmp(abc, abc) == 0);
        CHECK(ac_sv_cmp(e, abc) == -1);
        CHECK(ac_sv_cmp(abc, e) == 1);
        CHECK(ac_sv_cmp(e2, abc) == -1);
        CHECK(ac_sv_cmp(abc, e2) == 1);
        CHECK(ac_sv_starts_with(abc, "a"));
        CHECK(ac_sv_starts_with(abc, "ab"));
        CHECK(ac_sv_starts_with(abc, "abc"));
        CHECK_FALSE(ac_sv_starts_with(abc, "abcd"));
        CHECK_FALSE(ac_sv_starts_with(abc, "x"));
        CHECK_NULL(ac_sv_find_first(abc, 'x'));
        CHECK_NULL(ac_sv_find_last(abc, 'x'));
        const char* p = ac_sv_find_first(abc, 'b');
        CHECK_NOT_NULL(p);
        const char* p2 = ac_sv_find_last(abc, 'b');
        CHECK_EQ_PTR(p, p2);
        ac_sv bc = ac_make_sv_be(p, abc.end);
        EXPECT_SV("bc", bc);
    }

    {
        ac_sv foof = ac_make_sv_str("foof");
        CHECK_NULL(ac_sv_find_first(foof, 'x'));
        const char* p = ac_sv_find_first(foof, 'o');
        CHECK_NOT_NULL(p);
        ac_sv oof = ac_make_sv_be(p, foof.end);
        EXPECT_SV("oof", oof);
        const char* p2 = ac_sv_find_last(foof, 'o');
        CHECK_EQ_PTR(p + 1, p2);
        ac_sv fo = ac_make_sv_be(foof.begin, p2);
        EXPECT_SV("fo", fo);
    }

    {
        ac_sv lit = ac_make_sv_lit("asdf");
        CHECK_EQ(4, ac_sv_len(lit));
        EXPECT_SV("asdf", lit);
    }
}

bool cxx_to_c(ac_sv* a, ac_sv* b) {
    bool ret = ac_sv_cmp(*a, *b) == 0;
    --a->end;
    ++a->begin;
    *b = ac_make_sv_lit("hello from c");
    return ret;
}

static void cross(void) {
    ac_sv a = ac_make_sv_lit("c here");
    ac_sv b = a;
    c_to_cxx(&a, &b);
    EXPECT_SV("c++ here", a);
    EXPECT_SV("and again here", b);
}

static void printf_sv(void) {
    char buf[1024];
    ac_sv sv = ac_make_sv_str("hello");
    snprintf(buf, sizeof(buf), "text: " PRacsv, AC_PRINTF_SV(sv));
    CHECK_EQ_STR("text: hello", buf);
}

void run_unity_tests() {
    RUN_TEST(sv);
    RUN_TEST(cross);
    RUN_TEST(printf_sv);
}
