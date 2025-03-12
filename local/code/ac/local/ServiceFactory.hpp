// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <memory>

namespace ac::local {

struct ServiceInfo;
class Service;
class AppCtx;

class AC_LOCAL_EXPORT ServiceFactory {
public:
    virtual ~ServiceFactory();
    virtual const ServiceInfo& info() const noexcept = 0;
    virtual std::unique_ptr<Service> createService(const AppCtx& ctx) const = 0;
};

} // namespace ac::local
