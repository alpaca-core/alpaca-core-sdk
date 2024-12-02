// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/Field.hpp>
#include <vector>
#include <string>
#include <tuple>

namespace ac::local::schema {

struct DummyInterface {
    static constexpr auto id = "dummy/v1";

    struct OpRun {
        static constexpr auto id = "run";
        static constexpr auto desc = "Run the dummy inference and produce some output";

        struct Params {
            Field<std::vector<std::string>> input;
            Field<bool> splice = Default(true);
            Field<int> throwOn = Default(-1);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(input, "input", "Input items");
                v(splice, "splice", "Splice input with model data (otherwise append model data to input)");
                v(throwOn, "throw_on", "Throw exception on n-th token (or don't throw if -1)");
            }
        };
        struct Return {
            Field<std::string> result;

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(result, "result", "Output text (tokens joined with space)");
            }
        };
    };

    using Ops = std::tuple<OpRun>;
};

} // namespace ac::local::schema
