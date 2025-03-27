// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "FrameHelpers.hpp"
#include "Error.hpp"
#include "StateChange.hpp"
#include "OpTraits.hpp"

#include "../frameio/BlockingIo.hpp"
#include "../frameio/StreamEndpointFwd.hpp"
#include <astl/throw_stdex.hpp>
#include <astl/generator.hpp>

namespace ac::schema {

class BlockingIoHelper : private frameio::BlockingIo {
public:
    using super = frameio::BlockingIo;
    using super::blocking_io;

    static void pollStatusCheck(const io::status& s) {
        if (!s.success()) {
            throw_ex{} << "poll failed: " << s.bits;
        }
    }

    static void frameErrorCheck(Frame& frame) {
        if (auto err = Frame_optTo(Error{}, frame)) {
            throw_ex{} << "error: " << *err;
        }
    }

    Frame safePoll() {
        auto res = frameio::BlockingIo::poll();
        pollStatusCheck(res);
        frameErrorCheck(*res);
        return std::move(*res);
    }

    template <typename T>
    typename T::Type poll() {
        auto f = safePoll();
        return Frame_to(T{}, std::move(f));
    }

    template <typename Op>
    void initiate(typename Op::Params p) {
        auto status = push(Frame_from(OpParams<Op>{}, std::move(p)));
        if (!status.success()) {
            throw_ex{} << "push failed: " << status.bits;
        }
    }

    template <typename Op> requires (!Op_isStateTransition<Op>)
    typename Op::Return call(typename Op::Params p) {
        initiate<Op>(std::move(p));
        return this->poll<OpReturn<Op>>();
    }

    template <Op_isStateTransition Op>
    StateChange::Type call(typename Op::Params p) {
        initiate<Op>(std::move(p));
        return this->poll<StateChange>();
    }

    template <typename StreamOp, typename EndState = void>
    astl::generator<typename StreamOp::Type> runStream() {
        constexpr bool useEndState = !std::is_same_v<EndState, void>;
        std::optional<std::string_view> endState;
        if constexpr(useEndState) {
            endState = EndState::id;
        }

        while (true) {
            auto res = poll();
            pollStatusCheck(res);
            frameErrorCheck(*res);

            if (auto stateChange = Frame_optTo(StateChange{}, *res)) {
                if (endState.has_value() && stateChange != *endState) {
                    throw_ex{} << "wrong state: " << *stateChange << " (expected: " << *endState << ")";
                }
                co_return;
            }

            auto data = Frame_to(StreamOp{}, *res);
            co_yield data;
        }
    }

    using super::close;
};


} // namespace ac::schema
