// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation

@main
struct ACExample {
    static func main() {
        var desc = CAlpacaCore.ac.local.ModelDesc()
        desc.inferenceType = "dummy"
        desc.name = "synthetic dummy"

        var dict = Dictionary<String, Any>()
        dict["str"] = "hello"

        let modelFactory = AlpacaCore.getModelFactory()
        DummyInference.add(modelFactory)
        let model = AlpacaCore.createModel(desc, dict)
        let instance = model?.createInstance("general", [:])

        let params = [
            "input": ["a", "b", "c"]
        ]
        let result = instance?.runOp("run", params)
        print("Result from inference: \(result!["result"]!)")
    }
}
