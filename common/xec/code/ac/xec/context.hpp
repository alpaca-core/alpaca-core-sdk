// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "api.h"
#include "strand.hpp"
#include <memory>

namespace ac::xec {

class context_work_guard;

class AC_XEC_API context {
public:
    context();
    ~context();

    context(const context&) = delete;
    context& operator=(const context&) = delete;

    size_t run();

    size_t poll();

    void stop();
    bool stopped() const;
    void restart();

    context_work_guard make_work_guard();

    strand make_strand();

    struct impl;
    impl& _impl() { return *m_impl; }
private:
    std::unique_ptr<impl> m_impl;
};

} // namespace ac::xec
