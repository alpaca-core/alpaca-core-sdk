// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "IoPtr.hpp"
#include <astl/shared_from.hpp>
#include <memory>

namespace ac::frameio {

class AC_FRAME_EXPORT SessionHandler : public astl::enable_shared_from {
public:
    SessionHandler(const SessionHandler&) = delete;
    SessionHandler& operator=(const SessionHandler&) = delete;

protected:
    SessionHandler();
    // intentionally not virtual. Objects are not owned through this, but instead through shared pointers
    ~SessionHandler();

    virtual void shConnected() noexcept;

    Input& shInput() const noexcept { return *m_in; }
    Output& shOutput() const noexcept { return *m_out; }
    IoExecutor& shExecutor() const noexcept { return *m_executor; }

private:
    InputPtr m_in;
    OutputPtr m_out;
    IoExecutorPtr m_executor;

public:
    static void init(SessionHandler& self,
        InputPtr in,
        OutputPtr out,
        IoExecutorPtr executor
    ) noexcept;
};

} // namespace ac::frameio
