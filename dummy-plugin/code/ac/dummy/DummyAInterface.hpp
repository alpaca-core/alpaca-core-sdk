// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <vector>
#include <string>

namespace ac::local::schema {

struct DummyAInterface {
    static constexpr auto id = "dummy-a/v1";

    struct OpRun {
        static constexpr auto id = "run";
        static constexpr auto desc = "Run the dummy inference and produce some output";

        struct Params {
            std::vector<std::string> input;
            bool splice = false;
            int throwOn = -1;

            template <typename Visitor>
            void visit(Visitor& v) {
                v(input, "input", "Input items", true);
                v(splice, "splice", "Splice input with model data (otherwise append model data to input)");
                v(throwOn, "throwOn", "Throw exception on n-th token (or don't throw if -1)");
            }
        };
        struct Return {
            std::string result;

            template <typename Visitor>
            void visit(Visitor& v) {
                v(result, "result", "Output text (tokens joined with space)", true);
            }
        };
    };
};

} // namespace ac::local::schema
