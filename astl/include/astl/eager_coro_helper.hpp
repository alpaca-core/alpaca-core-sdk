// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <coroutine>
#include <stdexcept>

// eager coroutine helper as per: https://ibob.bg/blog/2024/10/06/coro-throw/

#if defined(__GNUC__)
#   if defined(__clang__)
#       if __clang_major__ > 19
#           error "Clang version > 19 is not tested"
#       endif
#   elif __GNUC__ > 14
#       error "GCC version > 14 is not tested"
#   endif
#endif

namespace astl {

struct eager_coro_helper {
    class promise {
        std::exception_ptr m_exception;
        bool m_has_been_suspended = false;
        bool m_has_pre_suspend_exceptions = false;
    public:
        void unhandled_exception() {
            if (m_has_been_suspended) {
                m_exception = std::current_exception();
            }
            else {
                m_has_pre_suspend_exceptions = true;
                throw;
            }
        }

        void on_suspend() noexcept {
            m_has_been_suspended = true;
        }

        void rethrow_if_exception() {
            if (m_exception) {
                std::rethrow_exception(m_exception);
            }
        }

        bool has_pre_suspend_exceptions() const noexcept {
            return m_has_pre_suspend_exceptions;
        }
    };

    template <typename PT>
    static void safe_destroy_handle(const std::coroutine_handle<PT>& h) noexcept {
        static_assert(std::is_base_of_v<eager_coro_helper::promise, PT>);
        if (h && !h.promise().has_pre_suspend_exceptions()) {
            h.destroy();
        }
    }
};

} // namespace astl
