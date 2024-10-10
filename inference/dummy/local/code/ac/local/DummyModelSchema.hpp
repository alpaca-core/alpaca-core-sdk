// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/local/ModelSchema.hpp>

namespace ac::local::schema {

struct Dummy : public ModelHelper<Dummy> {

    struct Params : public Object {
        using Object::Object;
        String spliceString{*this, "splice_string", "String to splice model data with input"};
    };

    struct InstanceGeneral : public InstanceHelper<InstanceGeneral> {
        static inline constexpr std::string_view id = "general";

        struct Params : public Object {
            using Object::Object;
            String cutoff{*this, "cutoff", "Cut off model data to n-th element (or don't cut if -1)", -1};
        };

        struct OpRun {
            static inline constexpr std::string_view id = "run";

            struct Params : public Object {
                using Object::Object;
                Array<String> input{*this, "input", "Input items"};
                Bool splice{*this, "splice", "Splice input with model data (otherwise append model data to input)", true};
                Int throwOn{*this, "throw_on", "Throw exception on n-th token (or don't throw if -1)", -1};
            };

            struct Return : public Object {
                using Object::Object;
                String result{*this, "output", "Output text (tokens joined with space)", {}, true};
            };
        };

        using Ops = std::tuple<OpRun>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

}  // namespace ac::local::schema
