// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

template<class T>
class IntrusiveRefCounted {
public:
    IntrusiveRefCounted()
        : referenceCount(1)
    {}

    IntrusiveRefCounted(const IntrusiveRefCounted &) = delete;

    void retain() {
        ++referenceCount;
    }

    void release() {
        --referenceCount;
        if (referenceCount == 0) {
            delete static_cast<T *>(this);
        }
    }
private:
    int referenceCount;
};
