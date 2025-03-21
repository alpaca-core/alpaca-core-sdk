// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#pragma once
#include "../API.h"
#include "../Sink.hpp"

namespace ac::jalog::sinks
{

class AC_JALOG_API WindowsColorSink final : public Sink
{
public:
    WindowsColorSink();
    virtual void record(const Entry& entry) override;

private:
    void* m_out;
    void* m_err;
};

}
