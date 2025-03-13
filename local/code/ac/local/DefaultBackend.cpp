// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "DefaultBackend.hpp"

#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/multi_thread_runner.hpp>

namespace ac::local {
namespace {
struct XctxHelper {
    xec::context ctx;
    xec::context_work_guard guard;
    xec::multi_thread_runner threads;

    explicit XctxHelper(std::string_view name, size_t numThreads = 1)
        : guard(ctx)
        , threads(ctx, numThreads, name)
    {}

    xec::strand make_strand() {
        return ctx.make_strand();
    }

    void stop(bool force) {
        guard.reset();
        if (force) {
            ctx.stop();
        }
    }
};
} // namespace

struct DefaultBackend::Impl {
    XctxHelper
        system{"ac-sys", 2},
        io{"ac-io", 2},
        dispatch{"ac-dsp", 2},
        cpu{"ac-cpu"},
        gpu{"ac-gpu"};
};

DefaultBackend::DefaultBackend()
    : m_impl(std::make_unique<Impl>())
{
    m_xctx.system = &m_impl->system.ctx;
    m_xctx.io = &m_impl->io.ctx;
    m_xctx.dispatch = &m_impl->dispatch.ctx;
    m_xctx.cpu = m_impl->cpu.make_strand();
    m_xctx.gpu = m_impl->gpu.make_strand();
}

DefaultBackend::~DefaultBackend() {
    join();
}

void DefaultBackend::join(bool forceStop) {
    m_impl->system.stop(forceStop);
    m_impl->io.stop(forceStop);
    m_impl->dispatch.stop(forceStop);
    m_impl->cpu.stop(forceStop);
    m_impl->gpu.stop(forceStop);

    m_xctx = {};

    m_impl.reset();
}

} // namespace ac::local
