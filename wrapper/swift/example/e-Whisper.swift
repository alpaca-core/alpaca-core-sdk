// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import AlpacaCore

func progress(_ progress: Float) {
    print("Progress: \(progress)")
}

@main
struct WhisperExample {
    static func main() {
        print("Hello from e-Whisper.swift")

        initSDK();

        var desc = AlpacaCore.ModelDesc()
        desc.inferenceType = "whisper.cpp"
        desc.name = "synthetic whisper"
        // // AC_TEST_DATA_WHISPER_DIR "/whisper-base.en-f16.bin"
        let whisperDir = "/Users/pacominev/repos/ac/alpaca-core/.cpm/ac-test-data-whisper/70a55b9fcc626b9333fb3f54efc7118a2ce230bd"
        desc.assets.append(AlpacaCore.AssetInfo(whisperDir + "/whisper-base.en-f16.bin", "whisper-base.en-f16.bin"))

        let params = Dictionary<String, Any>()
        let model = createModel(&desc, params, progress)!;
        let instance = model.createInstance("general", params);

        let audioFile = "/as-she-sat.wav" //AC_TEST_DATA_WHISPER_DIR "/as-she-sat.wav";
        // // auto pcmf32 = ac::audio::loadWavF32Mono(audioFile);
        // // auto audioBlob = convertF32ToBlob(pcmf32);

        print("Local-whisper: Transcribing the audio [\(audioFile)]: \n\n");
        var inferenceParams = Dictionary<String, Any>()
        let data = "Hello, World!".data(using: .utf8)!
        inferenceParams["audioBinaryMono"] = data

        let result = instance.runOp("transcribe", inferenceParams, progress);
        print("Result: \(result)")
    }
}
