// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import CAlpacaCore
import AlpacaCore

func progress(_ progress: Float) {
    print("Progress: \(progress)")
}

@main
struct DummyExample {
    static func main() {
        print("Hello from e-Dummy.swift")

        AlpacaCore.initSDK();

        var desc = AlpacaCore.ModelDesc()
        desc.inferenceType = "dummy"
        desc.name = "synthetic dummy"

        let dict = Dictionary<String, Any>()
        let model = AlpacaCore.createModel(&desc, dict, progress)!;
        let instance = model.createInstance("general", dict);

        let params = [
            "input": ["a", "b"]
        ]

        let result = instance.runOp("run", params, progress);
        print("Result \(result)")
    }
}
