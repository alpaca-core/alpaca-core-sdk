// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import AlpacaCoreSwift

func progress(_ tag: String, _ progress: Float) {
    print("[\(tag)] Progress: \(progress)")
}

@main
struct DummyExample {
    static func main() {
        print("Hello from e-Dummy.swift")

        AlpacaCoreSwift.initSDK();

        let model: AlpacaCoreSwift.Model
        let instance: AlpacaCoreSwift.Instance

        do {
            var desc = AlpacaCoreSwift.ModelDesc()
            desc.inferenceType = "dummy"
            desc.name = "synthetic dummy"

            let dict = Dictionary<String, Any>()
            model = try AlpacaCoreSwift.createModel(&desc, dict, progress);
            instance = try model.createInstance("general", dict);
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

        let params = [
            "input": ["a", "b"]
        ]

        do {
            let result = try instance.runOp("run", params, progress);
            print("Result \(result)")
        } catch {
            print("Error running operation: \(error)")
        }
    }
}
