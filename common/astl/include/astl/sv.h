// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "inline.h"
#include "empty_init.h"

#include <stdbool.h>
#include <string.h>

#if defined(__cplusplus)
#include <string_view>
#endif

// string view

typedef struct ac_sv
{
    const char* begin; // start of string
    const char* end; // one after the last character of the string

#if defined(__cplusplus)
    std::string_view to_std() const noexcept {
        return std::string_view(begin, end - begin);
    }
    static ac_sv from_std(const std::string_view& sv) noexcept {
        ac_sv ret;
        ret.begin = sv.data();
        ret.end = ret.begin + sv.length();
        return ret;
    }
    bool operator==(const std::string_view& sv) const noexcept {
        return to_std() == sv;
    }
    friend bool operator==(const std::string_view& sv, const ac_sv& d) noexcept {
        return sv == d.to_std();
    }
    bool operator==(const ac_sv& other) const noexcept {
        return to_std() == other.to_std();
    }
    bool operator!=(const std::string_view& sv) const noexcept {
        return to_std() != sv;
    }
    friend bool operator!=(const std::string_view& sv, const ac_sv& d) noexcept {
        return sv != d.to_std();
    }
    bool operator!=(const ac_sv& other) const noexcept {
        return to_std() != other.to_std();
    }
    bool operator<(const ac_sv& other) const noexcept {
        return to_std() < other.to_std();
    }
    bool empty() const noexcept {
        return begin == end;
    }
#endif
} ac_sv;

#if defined(__cplusplus)
extern "C" {
#endif

AC_INLINE ac_sv ac_make_sv_be(const char* begin, const char* end) {
    ac_sv ret = {begin, end};
    return ret;
}

AC_INLINE ac_sv ac_make_sv_len(const char* begin, size_t length) {
    return ac_make_sv_be(begin, begin + length);
}

AC_INLINE ac_sv ac_make_sv_str(const char* str) {
    if (!str) return AC_EMPTY_T(ac_sv);
    return ac_make_sv_be(str, str + strlen(str));
}

#define ac_make_sv_lit(lit) ac_make_sv_len(lit, sizeof(lit)-1)

AC_INLINE bool ac_sv_is_null(ac_sv sv) { return !sv.begin; }
AC_INLINE bool ac_sv_is_empty(ac_sv sv) { return sv.begin == sv.end; }
AC_INLINE size_t ac_sv_len(ac_sv sv) { return sv.end - sv.begin; }

AC_INLINE int ac_sv_cmp(ac_sv a, ac_sv b) {
    // avoid memcmp with null
    if (a.begin == b.begin) return 0;
    size_t alen = ac_sv_len(a);
    if (!b.begin) return !!alen;
    size_t blen = ac_sv_len(b);
    if (!a.begin) return -!!blen;

    if (alen == blen) {
        return memcmp(a.begin, b.begin, alen);
    }
    else if (alen < blen) {
        int cmp = memcmp(a.begin, b.begin, alen);
        return cmp == 0 ? -1 : cmp;
    }
    else {
        int cmp = memcmp(a.begin, b.begin, blen);
        return cmp == 0 ? 1 : cmp;
    }
}

AC_INLINE bool ac_sv_eq(ac_sv a, ac_sv b) {
    return ac_sv_cmp(a, b) == 0;
}

AC_INLINE bool ac_sv_starts_with(ac_sv sv, const char* prefix) {
    size_t svlen = ac_sv_len(sv);
    size_t plen = strlen(prefix);
    if (plen > svlen) return false; // prefix longer than string
    if (svlen == 0 && plen == 0) return true; // avoid memcmp with null
    return memcmp(sv.begin, prefix, plen) == 0;
}

AC_INLINE const char* ac_sv_find_first(ac_sv sv, char q) {
    size_t len = ac_sv_len(sv);
    if (!len) return NULL; // avoid memchr with null
    return (const char*)memchr(sv.begin, q, len);
}

AC_INLINE const char* ac_sv_find_last(ac_sv sv, char q) {
    size_t len = ac_sv_len(sv);
    while (len--) {
        if (sv.begin[len] == q) return sv.begin + len;
    }
    return NULL;

    // future:
    // if (!len) return NULL; // avoid memrchr with null
    // return (const char*)memrchr(sv.begin, q, len);
}

#if defined(__cplusplus)
}
#endif

// printf utils
#define PRacsv "%.*s"
#define AC_PRINTF_SV(sv) (int)ac_sv_len(sv), sv.begin
