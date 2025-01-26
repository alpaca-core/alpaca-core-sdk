// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include "DummyInterface.hpp"
#include <tuple>

namespace ac::schema {

struct DummyProvider {
    static inline constexpr std::string_view id = "dummy";
    static inline constexpr std::string_view description = "Dummy inference for tests, examples, and experiments.";

    struct Params {
        Field<std::string> filePath = std::nullopt;
        Field<std::string> spliceString = std::nullopt;

        template <typename Visitor>
        void visitFields(Visitor& v) {
            v(filePath, "file_path", "Optional path to the file with model data. Empty for synthetic data");
            v(spliceString, "splice_string", "String to splice between model data elements");
        }
    };

    struct InstanceGeneral {
        static inline constexpr std::string_view id = "general";
        static inline constexpr std::string_view description = "General instance";

        struct Params {
            Field<int> cutoff = Default(-1);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(cutoff, "cutoff", "Cut off model data to n-th element (or don't cut if -1)");
            }
        };

        using Interfaces = std::tuple<DummyInterface>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

} // namespace ac::schema
