// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include "Frame.hpp"
#include "SessionHandlerPtr.hpp"
#include "SessionExecutorPtr.hpp"
#include <astl/shared_from.hpp>
#include <optional>
#include <memory>
#include <functional>

namespace ac {
class Session;

class AC_API_EXPORT SessionHandler : public astl::enable_shared_from {
public:
    SessionHandler(const SessionHandler&) = delete;
    SessionHandler& operator=(const SessionHandler&) = delete;

    // check if the handler is connected to a session
    bool connected() const noexcept;

    ////////////////////////////////////////////////
    // session proxies

    // post a task to be executed on the session strand
    // THIS IS THE ONLY FUNCTION WHICH IS VALID ON ANY THREAD
    // ONLY CALL frame io functions from within a task posted to the session strand
    // posting a task will extend the lifetime of the posting handler until the task is complete
    // thus capturing [this] or members by ref, when posting from a derived class, is safe
    void postSessionStrandTask(std::function<void()> task);

    // check if the session has pending input frames
    bool sessionHasInFrames() const;

    // get the next input frame from the session (or nullopt if none)
    std::optional<Frame> getSessionInFrame();

    // check if the session is accepting output frames
    bool sessionAcceptsOutFrames() const;

    // push an output frame to the session
    // returns false and does not touch the frame if the session does not accept frames
    // otherwise consumes the frame and returns true
    bool pushSessionOutFrame(Frame&& frame);

    // close the session - disconnect
    void closeSession();

    ////////////////////////////////////////////////
    // handler interface
    // all functions are called on the session strand

    // called when the handler is attached to a session
    // prev is the previous handler, if any
    virtual void shAttached(const SessionHandlerPtr& prev);

    // called when the handler is detached from a session (replaced by another)
    // (not the same as the session being closed)
    // this method would usually not be overridden
    virtual void shDetached();

    // called when the session is closed (disconnected)
    virtual void shSessionClosed();

    // called when the session has input frames available
    // will be called once only if sessionHasInFrames returns false or getSessionInFrame returns nullopt
    virtual void shOnAvailableSessionInFrames();

    // called when the session is accepting output frames
    // will be called once only if sessionAcceptsOutFrames returns false or pushSessionOutFrame returns false
    virtual void shOnSessionAcceptsOutFrames();

protected:
    SessionHandler();
    // intentionally not virtual. Objects are not owned through this, but instead through shared pointers
    ~SessionHandler();

private:
    friend class Session;
    Session* m_session = nullptr;
    SessionExecutorPtr m_executor;
};

}
