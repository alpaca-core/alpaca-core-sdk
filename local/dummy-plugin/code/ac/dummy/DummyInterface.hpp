// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/schema/Field.hpp>
#include <vector>
#include <string>
#include <tuple>

namespace ac::schema {

inline namespace dummy {

struct StateInitial {
    static constexpr auto id = "dummy";
    static constexpr auto desc = "Initial dummy state";

    struct OpLoadModel {
        static constexpr auto id = "load_model";
        static constexpr auto desc = "Load the dummy model";

        struct Params {
            Field<std::string> filePath = std::nullopt;
            Field<std::string> spliceString = std::nullopt;

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(filePath, "file_path", "Optional path to the file with model data. Empty for synthetic data");
                v(spliceString, "splice_string", "String to splice between model data elements");
            }
        };

        using Return = nullptr_t;
    };

    using Ops = std::tuple<OpLoadModel>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<>;
};

struct StateLoadingModel {
    static constexpr auto id = "loading_model";
    static constexpr auto desc = "Loading the dummy model. After completion the state will transition to Model Loaded";

    struct OpAbort {
        static constexpr auto id = "abort";
        static constexpr auto desc = "Abort the model loading";
        using Params = nullptr_t;
        using Return = nullptr_t;
    };

    struct StreamProgress {
        static constexpr auto id = "progress";
        static constexpr auto desc = "Progress stream";

        struct Type {
            Field<int> progress;

            template <typename Visitor>
            void visitFields(Visitor& v) {
                v(progress, "progress", "Progress from 0 to 1");
            };
        };
    };

    using Ops = std::tuple<OpAbort>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<StreamProgress>;
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

        using Return = nullptr_t;
    };

    using Ops = std::tuple<OpCreateInstance>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<>;
};

struct StateStreaming {
    static constexpr auto id = "streaming";
    static constexpr auto desc = "Streaming state";

    struct OpAbort {
        static constexpr auto id = "abort";
        static constexpr auto desc = "Abort the streaming";
        using Params = nullptr_t;
        using Return = nullptr_t;
    };

    struct StreamToken {
        static constexpr auto id = "token";
        static constexpr auto desc = "Token stream";
        using Type = std::string;
    };

    using Ops = std::tuple<OpAbort>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<StreamToken>;
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
        using Return = nullptr_t;
    };

    using Ops = std::tuple<OpRun, OpStream>;
    using Ins = std::tuple<>;
    using Outs = std::tuple<>;
};

struct Interface {
    static inline constexpr std::string_view id = "dummy";
    static inline constexpr std::string_view desc = "Dummy inference for tests, examples, and experiments.";

    using States = std::tuple<StateInitial, StateLoadingModel, StateModelLoaded, StateInstance>;
};

} // namespace dummy

} // namespace ac::schema
