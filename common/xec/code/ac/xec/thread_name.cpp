// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "thread_name.hpp"

#include <cstring>
#include <cstdio>
#include <cassert>

#if defined(_WIN32) && !defined(_POSIX_THREADS)
#   define WIN32_THREADS 1
#else
#   define WIN32_THREADS 0
#endif

#if WIN32_THREADS
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
using tid = HANDLE;
#else
#   if defined(__ANDROID__)
#       include <sys/prctl.h>
#   endif
using tid = pthread_t;
#endif

namespace ac::xec {

namespace {
[[maybe_unused]] bool is_current_thread(tid h) {
#if WIN32_THREADS
    return h == GetCurrentThread();
#else
    return h == pthread_self();
#endif
}

int do_set_name(tid h, std::string_view name) {
    // max length for thread names with pthread is 16 symbols
    // even though we don't have the same limitation on Widows, we assert anyway for multiplatofm-ness
    // note that the length is safe-guarded below
    // you can remove this assertion and the thread names will be trimmed on unix
    // however it's here so we have a notification if we violate the rule
    assert(name.length() < 16);

#if WIN32_THREADS
    std::wstring ww;
    ww.reserve(name.length());
    for (auto c : name) {
        ww.push_back(wchar_t(c));
    }
    auto res = SetThreadDescription(h, ww.c_str());
    // hresult hax
    if (SUCCEEDED(res)) return 0;
    return res;
#else
    char name16[16];
    auto len = std::min(name.length() + 1, size_t(16));
    snprintf(name16, len, "%s", name.data());
#   if defined(__APPLE__)
    // stupid macs not letting us rename any thread but only the current one
    if (is_current_thread(h)) {
        return pthread_setname_np(name16);
    }
    else {
        return 1; // nothing smart to do, return error
    }
#   else
    return pthread_setname_np(h, name16);
#   endif
#endif
}

std::string do_get_name(tid h) {
    std::string name;
#if WIN32_THREADS
    PWSTR desc = nullptr;
    if (FAILED(GetThreadDescription(h, &desc))) return {};
    auto p = desc;
    while (*p) {
        name.push_back(char(*p));
        ++p;
    }
    LocalFree(desc);
#elif defined(__ANDROID__)
    // andoird doesn't have pthread_getname_np so we need to resort to prctl
    // unfortunately it only works for the calling thread
    if (is_current_thread(h)) {
        char name16[16] = {};
        prctl(PR_GET_NAME, name16);
        name = name16;
    }
    else {
        return {}; // nothing smart to do, return empty string
    }
#else
    char name16[17] = {};
    pthread_getname_np(h, name16, sizeof(name16));
    name = name16;
#endif
    return name;
}
}

int set_thread_name(std::thread& t, std::string_view name) noexcept {
    return do_set_name(t.native_handle(), name);
}

int set_this_thread_name(std::string_view name) noexcept {
#if WIN32_THREADS
    return do_set_name(GetCurrentThread(), name);
#else
    return do_set_name(pthread_self(), name);
#endif
}

std::string get_thread_name(std::thread& t) noexcept {
    return do_get_name(t.native_handle());
}

std::string get_this_thread_name() noexcept {
#if WIN32_THREADS
    return do_get_name(GetCurrentThread());
#else
    return do_get_name(pthread_self());
#endif
}

} // namespace ac::xec
