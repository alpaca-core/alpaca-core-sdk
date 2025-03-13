// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <astl/version.hpp>
#include <vector>

namespace ac::local {

class ServiceFactory;

struct PluginInterface {
    using GetAcLocalVersionFunc = int(*)();
    using GetFunc = PluginInterface(*)();

    const char* label; // optional own name (not necessarily unique)
    const char* desc; // optional short description
    const char* vendor; // optional vendor name
    astl::version version; // version of the plugin

    const char* const* tags; // optional tags
    int numTags; // number of tags

    void* rawData; // optional raw data

    using InitFunc = void(*)();
    InitFunc init; // optional init function

    using GetServiceFactories = std::vector<const ServiceFactory*>(*)();
    GetServiceFactories getServiceFactories; // function to get service factories
};

}
