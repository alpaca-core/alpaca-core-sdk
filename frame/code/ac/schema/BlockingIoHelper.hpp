// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "FrameHelpers.hpp"
#include "Error.hpp"
#include "Abort.hpp"
#include "StateChange.hpp"
#include "OpTraits.hpp"

#include "../frameio/BlockingIo.hpp"
#include "../frameio/StreamEndpointFwd.hpp"
#include <astl/throw_stdex.hpp>
#include <astl/generator.hpp>
#include <astl/sentry.hpp>

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

    template <typename Op>
    OpReturn<Op>::Type call(typename Op::Params p) {
        initiate<Op>(std::move(p));
        return this->poll<OpReturn<Op>>();
    }

    template <typename Op, size_t StreamIndex = 0>
    astl::generator<
        typename std::tuple_element<StreamIndex, typename Op::Outs>::type::Type,
        typename OpReturn<Op>::Type
    > stream(typename Op::Params params) {
        initiate<Op>(std::move(params));

        using Stream = typename std::tuple_element<StreamIndex, typename Op::Outs>::type;

        bool completedNormally = false;
        astl::sentry abortSentry([&] {
            if (!completedNormally) {
                push(Frame_from(Abort{}, {}));
            }
        });

        while (true) {
            auto f = safePoll();

            if (auto s = Frame_optTo(Stream{}, f)) {
                co_yield std::move(*s);
            }
            else if (auto ret = Frame_optTo(OpReturn<Op>{}, f)) {
                completedNormally = true;
                co_return std::move(*ret);
            }
            else {
                throw_ex{} << "unexpected frame: " << f.op;
            }
        }
    }

    using super::close;
};


} // namespace ac::schema
