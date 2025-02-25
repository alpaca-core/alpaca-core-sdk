// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "FrameHelpers.hpp"
#include "../frameio/local/BlockingIo.hpp"
#include "../frameio/StreamEndpoint.hpp"
#include "../FrameUtil.hpp"
#include <astl/throw_stdex.hpp>

namespace ac::schema {

class BlockingIoHelper {
    frameio::BlockingIo m_io;
public:
    explicit BlockingIoHelper(frameio::StreamEndpoint ep, frameio::BlockingIoCtx& ctx) : m_io(std::move(ep), ctx) {}

    frameio::BlockingIo& io() { return m_io; }

    static void pollStatusCheck(const io::status& s) {
        if (!s.success()) {
            throw_ex{} << "poll failed: " << s.bits;
        }
    }

    static void frameErrorCheck(const Frame& frame) {
        if (Frame_isError(frame)) {
            throw_ex{} << "error: " << Frame_getError(frame);
        }
    }

    template <typename State>
    void expectState() {
        auto res = m_io.poll();
        pollStatusCheck(res);
        auto state = Frame_getStateChange(res.value);
        if (state != State::id) {
            throw_ex{} << "unexpected state: " << state;
        }
    }

    template <typename State>
    void awaitState() {
        while (true) {
            auto res = m_io.poll();
            pollStatusCheck(res);
            frameErrorCheck(res.value);
            auto state = Frame_getStateChange(res.value);
            if (state == State::id) {
                return;
            }
        }
    }

    template <typename Op>
    typename Op::Return call(typename Op::Params p) {
        auto status = m_io.push(Frame_fromOpParams(Op{}, std::move(p)));
        if (!status.success()) {
            throw_ex{} << "push failed: " << status.bits;
        }
        auto res = m_io.poll();
        pollStatusCheck(res);
        frameErrorCheck(res.value);
        return Frame_toOpReturn(Op{}, std::move(res.value));
    }

    template <typename State, typename StreamOp, typename Cb>
    void runStream(Cb onResult) {
        expectState<State>();
        while (true) {
            auto res = m_io.poll();
            pollStatusCheck(res);
            frameErrorCheck(res.value);

            typename StreamOp::Type data;
            if (!Frame_getStreamData<StreamOp>(res.value, data)) {
                break;
            }
            onResult(data);
        }
    }

    void close() { m_io.close(); }
};


} // namespace ac::schema
