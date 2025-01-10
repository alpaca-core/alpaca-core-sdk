// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/Dict.hpp>
#include <optional>

namespace ac::local {

struct EncpointInfo {
    std::string info;
};

struct Frame {
    Dict data;
};

class Session {
public:
    // synchronous
    EncpointInfo getEndpointInfo();

    // concurrent calls are not supported
    std::optional<Frame> getInFrame(int msTimeout = -1);

    // synchronous: guarantees that no calls to previous callback will be made after this call completes
    // callback will be called on a different strand (potentially concurrently with the caller one)
    void setInFrameAvailableCb(std::function<void()> cb);

    // return frame id
    // synchronicity is determined by the implementation
    // concurrent calls are not supported
    uint64_t pushOutFrame(Frame frame);

    // synchronous: guarantees that no calls to previous callback will be made after this call completes
    // callback will be called on a different strand (potentially concurrently with the caller one)
    void setOutAckCb(std::function<void(uint64_t, const Frame&)> cb);

    // gets the last unprocessed frames (if any)
    // returned span is the argument, but resized to the number successfully "unput" frames
    // std::span<Frame> pullOutFrames(std::span<Frame> frames);

    // synchronously return last processed input frame id
    // can be used to deal with backpressure
    uint64_t getOutAckCount();

    // synchronous: guarantees that no calls to previous callback will be made after this call completes
    // callback will be called on a different strand (potentially concurrently with the caller one)
    // returns true when all frames are processed
    bool syncOutAck(int timeout = -1);

    // synchronous: no calls to frame callbacks will be made after this call completes
    // getFrame will return empty frames after this call completes
    // putFrame is noop after this call completes
    void close();
};

} // namespace ac::local
