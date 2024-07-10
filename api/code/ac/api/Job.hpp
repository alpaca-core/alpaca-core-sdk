// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"

namespace ac::api {

class AC_API_EXPORT Job {
public:
    virtual ~Job();

    // job ops are queued and executed in order
    // the inner job state is modified by ops and subsequent ones may depend on the results of previous ones
    virtual void runOp(std::string_view op, Dict params, Callback<Dict> cb) = 0;

    // wait for all ops to finish
    virtual void synchronize() = 0;

    // Note that some callbacks may still be called after this function.
    // Either wait for pending callbacks or call synchronize after this to guarantee that no more callbacks will be called.
    virtual void initiateAbort(Callback<void> cb) = 0;
};

}
