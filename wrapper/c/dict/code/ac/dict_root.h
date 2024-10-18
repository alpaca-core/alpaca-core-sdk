// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

/// An opaque pointer to an instance of a dictionary.
/// This is an owning pointer which must be freed with @ref ac_dict_free_root.
///
/// @ingroup c-dict
typedef struct ac_dict_root ac_dict_root;
