// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "export.h"
#include <ac/Dict.hpp>
#include <string_view>
#include <vector>

namespace ac::local {

/// A Vector Store.
/// @ingroup cpp-local
class AC_LOCAL_EXPORT VectorStore {
public:
    virtual ~VectorStore();

    // Adds records to the store
    // Requires a vector of records and their ids
    virtual void addRecords(ac::Dict records) = 0;
    virtual void removeRecords(ac::Dict ids) = 0;

    // Returns a record by id
    virtual ac::Dict get(ac::Dict param) = 0;

    // Returns a list of ids and scores
    // Requires a vector and a top K
    virtual ac::Dict search(ac::Dict params) = 0;
};
} // namespace ac::local
