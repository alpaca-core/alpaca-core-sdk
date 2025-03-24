// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

#include <ac/frameio/StreamEndpointFwd.hpp>
#include <ac/Dict.hpp>

#include <memory>

namespace ac::local {

struct ServiceInfo;

class AC_LOCAL_EXPORT Service {
public:
    virtual ~Service();

    virtual const ServiceInfo& info() const noexcept = 0;
    virtual void createSession(frameio::StreamEndpoint ep, Dict target) = 0;
};

} // namespace ac::local
