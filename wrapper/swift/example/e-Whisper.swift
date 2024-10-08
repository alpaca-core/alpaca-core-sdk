// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
// import CWhisper

@main
struct WhisperExample {
    static func main() {
        print("Hello from e-Whisper.swift")


        // ac::local::ModelFactory factory;
        // ac::local::addWhisperInference(factory);

        // var desc = CxxACLocal.ac.local.ModelDesc()
        // desc.inferenceType = "whisper"
        // desc. = "synthetic whisper"
        // var asset = CxxACLocal.ac.local.ModelDesc.AssetInfo()
        // asset.path =  AC_TEST_DATA_WHISPER_DIR "/whisper-base.en-f16.bin"
        // desc.assets.push_back(asset)

        // auto model = factory.createModel(desc, {}, progress);

        // auto instance = model->createInstance("general", {});

        let audioFile = "/as-she-sat.wav" //AC_TEST_DATA_WHISPER_DIR "/as-she-sat.wav";
        // auto pcmf32 = ac::audio::loadWavF32Mono(audioFile);
        // auto audioBlob = convertF32ToBlob(pcmf32);

        print("Local-whisper: Transcribing the audio [\(audioFile)]: \n\n");
        //let dict = ac.local.SwiftDict

        // auto result = instance->runOp("transcribe", {{"audioBinaryMono", ac::Dict::binary(std::move(audioBlob))}}, {});

        // std::cout << result.at("result").get<std::string_view>() << '\n';

    }
}
