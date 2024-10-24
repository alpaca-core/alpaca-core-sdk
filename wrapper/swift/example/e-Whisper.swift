// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import AlpacaCoreSwift
import ACTestData
import ACAudio

func progress(_ tag: String, _ progress: Float) {
    print("[\(tag)]Progress: \(progress)")
}

@main
struct WhisperExample {
    static func main() {
        print("Hello from e-Whisper.swift")

        initSDK();
        let model: Model
        let instance: Instance
        let whisperDir = String(cString: ACTestData.AC.getWhisperDir())
        do {
            var desc = ModelDesc()
            desc.inferenceType = "whisper.cpp"
            desc.name = "synthetic whisper"
            desc.assets.append(AssetInfo(whisperDir + "/whisper-base.en-f16.bin", "whisper-base.en-f16.bin"))

            let params = Dictionary<String, Any>()
            model = try createModel(&desc, params, progress);
            instance = try model.createInstance("general", params);
        } catch ACError.invalidModelCreation(let error) {
            print("Error creating model: \(error)")
            return
        } catch ACError.invalidInstanceCreation(let error) {
            print("Error creating instance: \(error)")
            return
        } catch {
            print("Unexpected error: \(error)")
            return
        }

        let audioFile = "/as-she-sat.wav"

        let filePath = whisperDir + audioFile
        let wavAudio = Array<Float>(ACAudio.AC.loadWavF32Mono(std.string(filePath)))
        let audioData = wavAudio.withUnsafeBufferPointer { bufferPointer in
            Data(buffer: bufferPointer)
        }

        var inferenceParams = Dictionary<String, Any>()
        inferenceParams["audioBinaryMono"] = audioData

        print("Local-whisper: Transcribing the audio [\(audioFile)]: \n\n")
        do {
            let result = try instance.runOp("transcribe", inferenceParams, progress);
            print("Result: \(result)")
        } catch {
            print("Error running operation: \(error)")
        }
    }
}
