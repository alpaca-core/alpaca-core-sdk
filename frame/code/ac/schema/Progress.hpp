// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "SysFrameOpPrefix.h"
#include "Field.hpp"
#include <string>

namespace ac::schema {

inline namespace sys {

struct Progress {
    static constexpr auto id = AC_SYSFRAME_OP_PREFIX "progress";

    struct Type {
        Field<float> progress;
        Field<std::string> tag = Default();
        Field<std::string> action = Default();

        template <typename Visitor>
        void visitFields(Visitor& v) {
            v(progress, "progress", "0-1 progress");
            v(tag, "tag", "What is being processed");
            v(action, "action", "What is being done");
        }
    };
};

} // namespace sys

} // namespace ac::schema
