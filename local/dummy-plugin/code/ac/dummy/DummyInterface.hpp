// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/local/schema/AssetInfo.hpp>
#include <ac/schema/Field.hpp>
#include <ac/schema/Progress.hpp>
#include <ac/schema/Abort.hpp>
#include <ac/schema/StateChange.hpp>
#include <vector>
#include <string>
#include <tuple>

namespace ac::schema {

inline namespace dummy {

struct StateDummy {
    static constexpr auto id = "dummy";
    static constexpr auto desc = "Initial dummy state";

    struct OpLoadModel {
        static constexpr auto id = "load_model";
        static constexpr auto desc = "Load the dummy model";

        struct Params {
            Field<AssetInfos> assets = std::nullopt;
            Field<std::string> spliceString = std::nullopt;

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(assets, "assets", "Model assets. Synthetic model if empty");
                v(spliceString, "splice_string", "String to splice between model data elements");
            }
        };

        using Return = StateChange;

        using Ins = std::tuple<>;
        using Outs = std::tuple<sys::Progress>;
    };

    using Ops = std::tuple<OpLoadModel>;
};

struct StateModelLoaded {
    static constexpr auto id = "model_loaded";
    static constexpr auto desc = "Model loaded";

    struct OpCreateInstance {
        static constexpr auto id = "create_instance";
        static constexpr auto desc = "Create an instance of the dummy model";

        struct Params {
            Field<int> cutoff = Default(-1);

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(cutoff, "cutoff", "Cut off model data to n-th element (or don't cut if -1)");
            }
        };

        using Return = StateChange;
    };

    using Ops = std::tuple<OpCreateInstance>;
};

struct StateInstance {
    static constexpr auto id = "instance";
    static constexpr auto desc = "Dummy instance";

    struct InferenceParams {
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

    struct OpRun {
        static constexpr auto id = "run";
        static constexpr auto desc = "Run the dummy inference and produce some output";

        using Params = InferenceParams;
        struct Return {
            Field<std::string> result;

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(result, "result", "Output text (tokens joined with space)");
            }
        };
    };

    struct OpStream {
        static constexpr auto id = "stream";
        static constexpr auto desc = "Run the dummy inference and stream the output";

        using Params = InferenceParams;

        struct StreamToken {
            static constexpr auto id = "token";
            static constexpr auto desc = "Token stream";
            using Type = std::string;
        };

        using Return = nullptr_t;
        using Outs = std::tuple<StreamToken>;
    };

    using Ops = std::tuple<OpRun, OpStream>;
};

struct Interface {
    static inline constexpr std::string_view id = "dummy";
    static inline constexpr std::string_view desc = "Dummy inference for tests, examples, and experiments.";
};

} // namespace dummy

} // namespace ac::schema
