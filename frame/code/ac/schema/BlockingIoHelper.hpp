// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "FrameHelpers.hpp"
#include "Error.hpp"
#include "StateChange.hpp"

#include "../frameio/BlockingIo.hpp"
#include "../frameio/StreamEndpointFwd.hpp"
#include <astl/throw_stdex.hpp>
#include <astl/generator.hpp>

namespace ac::schema {

class BlockingIoHelper {
    frameio::BlockingIo m_io;
public:
    explicit BlockingIoHelper(frameio::StreamEndpoint ep) : m_io(std::move(ep)) {}

    frameio::BlockingIo& io() { return m_io; }

    static void pollStatusCheck(const io::status& s) {
        if (!s.success()) {
            throw_ex{} << "poll failed: " << s.bits;
        }
    }

    static void frameErrorCheck(const Frame& frame) {
        if (auto err = Frame_optTo(Error{}, frame)) {
            throw_ex{} << "error: " << *err;
        }
    }

    template <typename State>
    void expectState() {
        auto res = m_io.poll();
        pollStatusCheck(res);

        auto state = Frame_to(StateChange{}, std::move(res.value));
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

            if (auto state = Frame_to(StateChange{}, std::move(res.value))) {
                return;
            }
        }
    }

    template <typename Op>
    typename Op::Return call(typename Op::Params p) {
        auto status = m_io.push(Frame_from(OpParams<Op>{}, std::move(p)));
        if (!status.success()) {
            throw_ex{} << "push failed: " << status.bits;
        }
        auto res = m_io.poll();
        pollStatusCheck(res);
        frameErrorCheck(res.value);
        return Frame_to(OpReturn<Op>{}, std::move(res.value));
    }

    template <typename StreamOp, typename EndState = void>
    astl::generator<typename StreamOp::Type> runStream() {
        constexpr bool useEndState = !std::is_same_v<EndState, void>;
        std::optional<std::string_view> endState;
        if constexpr(useEndState) {
            endState = EndState::id;
        }

        while (true) {
            auto res = m_io.poll();
            pollStatusCheck(res);
            frameErrorCheck(res.value);

            if (auto stateChange = Frame_optTo(StateChange{}, res.value)) {
                if (endState.has_value() && stateChange != endState.value()) {
                    throw_ex{} << "wrong state: " << *stateChange << " (expected: " << endState.value() << ")";
                }
                co_return;
            }

            auto data = Frame_to(StreamOp{}, res.value);
            co_yield data;
        }
    }

    void close() { m_io.close(); }
};


} // namespace ac::schema
