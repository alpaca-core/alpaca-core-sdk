// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import CxxACLocal
import CxxDummyInference
import AlpacaCore
import CxxAlpacaCoreDict

func progress(tag: Optional<String>, progress: Float) -> Bool {
    print("[\(tag!)] progress: \(progress)\n")

    return true
}

@main
struct CxxDummyExample {
    static func main() {
        var desc = CxxACLocal.ac.local.ModelDesc()
        desc.inferenceType = "dummy"
        desc.name = "synthetic dummy"
        var asset = CxxACLocal.ac.local.ModelDesc.AssetInfo()
        asset.path = "dummy"
        desc.assets.push_back(asset)

        var arr =  ["1", "2"]
        arr.append("3")

        var acDict = ac.SwiftACDict()
        let jsonString: String = """
            {
                "name": "Alice",
                "age": 28,
                "height": 1.6,
                "isMember": true,
                "scores": [88, 92, 79],
                "address": {
                    "city": "Wonderland",
                    "postalCode": "12345"
                }
            }
        """

        acDict.parseJson(jsonString, UInt32(jsonString.lengthOfBytes(using: .utf8)))
        print("Dict as string: \n\t \(acDict.dump())")

        // Since ModelFactory has deleted it's cpy-ctor, we can't use it in Swift
        // let modelFactory = CxxACLocal.ac.local.ModelFactory()
        // CxxDummyInference.addDummyInference(modelFactory)

        // let model = AlpacaCore.createModel(desc, dict, progress)
        // let instance = model?.createInstance("general", [:])

        // let params = [
        //     "input": ["a", "b", "c"]
        // ]
        // let result = instance?.runOp("run", params, progress)
        // print("Result from inference: \"\(result!["result"]!)\"\n")

        // AlpacaCore.release(instance)
        // AlpacaCore.release(model)
    }
}
