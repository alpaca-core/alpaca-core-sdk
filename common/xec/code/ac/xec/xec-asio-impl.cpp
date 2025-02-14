// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "context.hpp"
#include "context_work_guard.hpp"
#include "post.hpp"
#include "timer.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/steady_timer.hpp>

#include <astl/make_ptr.hpp>

namespace asio = boost::asio;

namespace ac::xec {

struct context::impl {
    asio::io_context ctx;
};

struct context_work_guard::impl {
    asio::executor_work_guard<asio::io_context::executor_type> guard;
};

context_work_guard::context_work_guard() = default;
context_work_guard::context_work_guard(context& ctx)
    : m_impl(astl::make_unique(impl{asio::make_work_guard(ctx._impl().ctx.get_executor())}))
{}
context_work_guard::~context_work_guard() = default;

context_work_guard::context_work_guard(context_work_guard&&) noexcept = default;
context_work_guard& context_work_guard::operator=(context_work_guard&&) noexcept = default;

void context_work_guard::reset() {
    m_impl.reset();
}

struct strand_impl {
    asio::strand<asio::io_context::executor_type> strand;
};

context::context()
    : m_impl(std::make_unique<impl>())
{}

context::~context() = default;

void context::run() {
    m_impl->ctx.run();
}

void context::stop() {
    m_impl->ctx.stop();
}

bool context::stopped() const {
    return m_impl->ctx.stopped();
}

void context::restart() {
    m_impl->ctx.restart();
}

context_work_guard context::make_work_guard() {
    return context_work_guard(*this);
}

strand context::make_strand() {
    return astl::make_shared(strand_impl{asio::make_strand(m_impl->ctx.get_executor())});
}

strand make_strand(context& ctx) {
    return ctx.make_strand();
}

void post(const strand& s, ufunc<void()> f) {
    asio::post(s->strand, std::move(f));
}

void post(context& ctx, ufunc<void()> f) {
    asio::post(ctx._impl().ctx, std::move(f));
}

timer::timer() = default;
timer::~timer() = default; // export vtable

struct timer_impl final : public timer {
public:
    asio::steady_timer m_timer;

    explicit timer_impl(strand_impl& s) : m_timer(s.strand) {}

    virtual size_t expire_after(duration timeFromNow) override {
        return m_timer.expires_after(timeFromNow);
    }
    virtual size_t expire_at(time_point t) override {
        return m_timer.expires_at(t);
    }
    virtual size_t expire_never() override {
        return m_timer.expires_at(time_point::max());
    }

    virtual size_t cancel() override {
        return m_timer.cancel();
    }
    virtual size_t cancel_one() override {
        return m_timer.cancel_one();
    }

    virtual time_point expiry() const override {
        return m_timer.expiry();
    }

    virtual void add_wait_cb(wait_func cb) override {
        m_timer.async_wait(std::move(cb));
    }
};

timer_ptr timer::create(const strand& s) {
    return std::make_unique<timer_impl>(*s);
}

} // namespace ac::xec
