// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../export.h"
#include "SessionHandlerPtr.hpp"
#include "IoPtr.hpp"
#include <astl/shared_from.hpp>
#include <memory>

namespace ac::frameio {

class AC_FRAME_EXPORT SessionHandler : public astl::enable_shared_from {
public:
    SessionHandler(const SessionHandler&) = delete;
    SessionHandler& operator=(const SessionHandler&) = delete;

protected:
    SessionHandler() noexcept;
    // intentionally not virtual. Objects are not owned through this, but instead through shared pointers
    ~SessionHandler();

    virtual void shConnected() noexcept;

    Input& shInput() const noexcept { return *m_in; }
    Output& shOutput() const noexcept { return *m_out; }
    IoExecutor& shExecutor() const noexcept { return *m_executor; }

    // sets a handler which will succeed (be attached to) this one's io once this is destroyed
    void setSuccessor(SessionHandlerPtr successor) noexcept {
        m_successor = std::move(successor);
    }

private:
    InputPtr m_in;
    OutputPtr m_out;
    IoExecutorPtr m_executor;

    SessionHandlerPtr m_successor;

public:
    // initializes the session handler with the given input, output, and executor
    // will post a shConnected event to the session handler via the executor
    static void init(const SessionHandlerPtr& handler,
        InputPtr in,
        OutputPtr out,
        IoExecutorPtr executor
    ) noexcept;
};

} // namespace ac::frameio
