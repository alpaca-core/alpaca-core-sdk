// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "schema/AssetMgrInterface.hpp"
#include "Backend.hpp"
#include <ac/schema/OpTraits.hpp>
#include <ac/schema/FrameHelpers.hpp>
#include <ac/schema/Error.hpp>
#include <ac/schema/Progress.hpp>
#include <ac/schema/AbortUtil.hpp>
#include <ac/frameio/IoEndpoint.hpp>
#include <ac/xec/coro.hpp>

namespace ac::local {

xec::coro<schema::AssetInfos> AssetMgr_makeAssetsAvailable(Backend& backend, frameio::IoEndpoint& io, std::vector<schema::AssetInfo> assets) {
    frameio::IoEndpoint amgrio(backend.connect(schema::amgr::Interface::id, {}), io.get_executor());
    co_await amgrio.poll(); // state change

    using AmgrSchema = schema::amgr::State;

    co_await amgrio.push(Frame_from(schema::OpParams<AmgrSchema::OpMakeAssetsAvailable>{}, std::move(assets)));

    while (true) {
        auto res = co_await amgrio.poll();
        if (auto f = Frame_optTo(schema::Error{}, *res)) {
            throw std::runtime_error(*f);
        }
        else if (Frame_is(schema::Progress{}, *res)) {
            io.put(*res);
        }
        else if (auto ret = Frame_optTo(schema::OpReturn<AmgrSchema::OpMakeAssetsAvailable>{}, * res)) {
            co_return std::move(*ret);
        }

        if (schema::Abort_check(io)) {
            throw std::runtime_error("aborted");
        }
    }
}

}
