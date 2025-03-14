// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "DefaultBackend.hpp"

#include <ac/xec/context_work_guard.hpp>
#include <ac/xec/multi_thread_runner.hpp>

namespace ac::local {
namespace impl {
namespace {
struct XctxHelper {
    xec::context ctx;
    xec::context_work_guard guard;
    xec::multi_thread_runner threads;

    explicit XctxHelper(std::string_view name, size_t numThreads = 1)
        : guard(ctx)
        , threads(ctx, numThreads, name)
    {
    }

    ~XctxHelper() {
        guard.reset();
    }

    xec::strand make_strand() {
        return ctx.make_strand();
    }
};
} // namespace

struct DefaultBackendXctx::Impl {
    XctxHelper
        system{"ac-sys", 2},
        io{"ac-io", 2},
        dispatch{"ac-dsp", 2},
        cpu{"ac-cpu"},
        gpu{"ac-gpu"};
};

DefaultBackendXctx::DefaultBackendXctx() : m_impl(std::make_unique<Impl>()) {}
DefaultBackendXctx::~DefaultBackendXctx() = default;
} // namespace impl

DefaultBackend::DefaultBackend(std::string_view name)
    : Backend(
        name,
        Xctx{
            .system = m_impl->system.ctx,
            .io = m_impl->io.ctx,
            .dispatch = m_impl->dispatch.ctx,
            .cpu = m_impl->cpu.make_strand(),
            .gpu = m_impl->gpu.make_strand()
        }
    )
{}

} // namespace ac::local
