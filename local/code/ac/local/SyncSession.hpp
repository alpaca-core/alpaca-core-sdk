// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/Session.hpp>
#include <ac/SessionHandlerPtr.hpp>

namespace ac::local {

// sync session useful for tests and trivial examples
// it is not intended to be used in production code
class AC_LOCAL_EXPORT SyncSession final : public Session {
public:
    SyncSession(SessionHandlerPtr handler);
    ~SyncSession();

    void put(Frame&& frame);
    std::optional<Frame> get() noexcept;
    bool valid() const noexcept;

private:
    bool hasInFrames() const override;
    std::optional<Frame> getInFrame() override;

    bool acceptOutFrames() const override;
    bool pushOutFrame(Frame&& frame) override;

    void close() override;

    SessionHandlerPtr m_handler;
    std::optional<Frame> m_inFrame;
    std::optional<Frame> m_outFrame;

    void runTasks();
};

}
