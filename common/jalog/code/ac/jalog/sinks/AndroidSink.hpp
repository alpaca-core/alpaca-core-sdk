// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../API.h"
#include "../Sink.hpp"

namespace ac::jalog::sinks
{

class AC_JALOG_API AndroidSink final : public Sink
{
public:
    virtual void record(const Entry& entry) override;
};

}
