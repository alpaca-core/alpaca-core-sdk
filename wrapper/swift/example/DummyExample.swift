// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation

func progress(tag: Optional<String>, progress: Float) -> Bool {
    print("[\(tag!)] progress: \(progress)\n")

    return true
}

@main
struct DummyExample {
    static func main() {
        var desc = NativeAlpacaCore.ac.local.ModelDesc()
        desc.inferenceType = "dummy"
        desc.name = "synthetic dummy"

        var dict = Dictionary<String, Any>()
        dict["str"] = "hello"

        let modelFactory = AlpacaCore.getModelFactory()
        DummyInference.add(modelFactory)
        let model = AlpacaCore.createModel(desc, dict, progress)
        let instance = model?.createInstance("general", [:])

        let params = [
            "input": ["a", "b", "c"]
        ]
        let result = instance?.runOp("run", params, progress)
        print("Result from inference: \"\(result!["result"]!)\"\n")

        AlpacaCore.release(instance)
        AlpacaCore.release(model)
    }
}
