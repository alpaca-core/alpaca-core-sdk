// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "ResourceManager.hpp"
#include <ac/xec/strand.hpp>
#include <ac/xec/timer_wobj.hpp>
#include <string>

namespace ac::local {
class Backend;

class BackendWorkerStrand {
public:
    BackendWorkerStrand(Backend& backend, std::string name, const xec::strand& ex)
        : backend(backend)
        , name(std::move(name))
        , m_wobj(ex)
    {}

    Backend& backend;

    const std::string name;
    ResourceManager resourceManager;

    const xec::strand& executor() const { return m_wobj.get_executor(); }

protected:
    friend class Backend;
    ac::xec::timer_wobj m_wobj;
};

} // namespace ac::local
