// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import AlpacaCoreSwift

func progress(_ progress: Float) {
    print("Progress: \(progress)")
}

@main
struct DummyExample {
    static func main() {
        print("Hello from e-Dummy.swift")

        AlpacaCoreSwift.initSDK();

        var desc = AlpacaCoreSwift.ModelDesc()
        desc.inferenceType = "dummy"
        desc.name = "synthetic dummy"

        let dict = Dictionary<String, Any>()
        let model = AlpacaCoreSwift.createModel(&desc, dict, progress)!;
        let instance = model.createInstance("general", dict);

        let params = [
            "input": ["a", "b"]
        ]

        let result = instance.runOp("run", params, progress);
        print("Result \(result)")
    }
}
