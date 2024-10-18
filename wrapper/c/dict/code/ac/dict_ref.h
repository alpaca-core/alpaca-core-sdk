// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once

/// An opaque pointer to a dictionary reference.
/// A dict ref is a non owning reference to an element in a dictionary. Its lifetime is tied to an @ref ac_dict_root.
///
/// @ingroup c-dict
typedef struct tag_ac_dict_ref* ac_dict_ref;
