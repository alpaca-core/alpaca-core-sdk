// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <ac/schema/ModelSchema.hpp>

namespace ac::local::schema {

struct Whisper : public ModelHelper<Whisper> {
    static inline constexpr std::string_view id = "whisper.cpp";
    static inline constexpr std::string_view description = "Inference based on our fork of https://github.com/ggerganov/whisper.cpp";

    using Params = Null;

    struct InstanceGeneral : public InstanceHelper<InstanceGeneral> {
        static inline constexpr std::string_view id = "general";
        static inline constexpr std::string_view description = "General instance";

        using Params = Null;

        struct OpTranscribe {
            static inline constexpr std::string_view id = "transcribe";
            static inline constexpr std::string_view description = "Run the llama.cpp inference and produce some output";

            struct Params : public Object {
                using Object::Object;
                Binary audio{*this, "audioBinaryMono", "Audio data to transcribe", {}, true};
            };

            struct Return : public Object {
                using Object::Object;
                String result{*this, "result", "Transcription of audio", {}, true};
            };
        };

        using Ops = std::tuple<OpTranscribe>;
    };

    using Instances = std::tuple<InstanceGeneral>;
};

}  // namespace ac::local::schema
