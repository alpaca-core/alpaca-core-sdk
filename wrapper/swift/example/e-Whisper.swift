// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import AlpacaCoreSwift

func progress(_ tag: String, _ progress: Float) {
    print("[\(tag)]Progress: \(progress)")
}

@main
struct WhisperExample {
    static func main() {
        print("Hello from e-Whisper.swift")

        initSDK();

        var desc = ModelDesc()
        desc.inferenceType = "whisper.cpp"
        desc.name = "synthetic whisper"
        let whisperDir = getWhisperDataDir()
        desc.assets.append(AssetInfo(whisperDir + "/whisper-base.en-f16.bin", "whisper-base.en-f16.bin"))

        let params = Dictionary<String, Any>()
        let model = createModel(&desc, params, progress)!;
        let instance = model.createInstance("general", params);

        let audioFile = "/as-she-sat.wav"

        let filePath = whisperDir + audioFile
        let wavAudio = loadWavF32Mono(filePath)
        let audioData = wavAudio.withUnsafeBufferPointer { bufferPointer in
            Data(buffer: bufferPointer)
        }

        var inferenceParams = Dictionary<String, Any>()
        inferenceParams["audioBinaryMono"] = audioData

        print("Local-whisper: Transcribing the audio [\(audioFile)]: \n\n")
        let result = instance.runOp("transcribe", inferenceParams, progress);
        print("Result: \(result)")
    }
}
