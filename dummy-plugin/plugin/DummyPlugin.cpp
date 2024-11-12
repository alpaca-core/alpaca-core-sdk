// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/dummy/LocalDummy.hpp>
#include <ac/local/PluginInterface.hpp>
#include <astl/symbol_export.h>

#include "version.h"

namespace ac::local {

extern "C" SYMBOL_EXPORT
PluginInterface acLocalPluginLoad() {
    constexpr Version ownVersion(
        ACLP_dummy_VERSION_MAJOR, ACLP_dummy_VERSION_MINOR, ACLP_dummy_VERSION_PATCH, ACLP_dummy_VERSION_TAG
    );

    return {
        .acLocalVersion = ac::local::Project_Version,
        .pluginVersion = ownVersion,
        .addLoadersToFactory = dummy::addToModelFactory,
    };
}
static_assert(std::is_same_v<decltype(&acLocalPluginLoad), PluginInterface::PluginLoadFunc>);

} // namespace ac::local
