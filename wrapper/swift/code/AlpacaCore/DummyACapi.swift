// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
//import CAlpacaCore
import Foundation

public func createModel1(modelDescription: CAlpacaCore.ac.local.ModelDesc) -> ACModel! {
    let smallDictionary: [String: Any] = [
        "name": "Alice",
        "age": 28,
        "height": 1.7,
        "isMember": true,
        "scores": [88, 92, 79],
        "inner": [
            "str": "hello",
            "float": 3.14,
            "neg": -100,
            "big": 3000000000,
            "more_inner": [
                "str": "hello",
                "float": 3.14,
                "neg": -100,
                "big": 3000000000
            ]
        ],
        "empty_list": [],
        "empty_dict": [:],
        "empty_object": {}
    ]

    let data = NSString()
    var model = createModel(modelDescription, smallDictionary)
    model?.createInstance()

    return createModel(modelDescription, smallDictionary)
}

public func test() {
    var data = Data() // "{\"pesho\": \"Hello, World!\"}".data(using: .utf8)!
    data.append("\"5\": 5".data(using: .utf8)!)

    let smallDictionary: [String: Any] = [
        "name": "Alice",
        "age": 28,
        "height": 1.7,
        "isMember": true,
        "scores": [88, 92, 79],
        "inner": [
            "str": "hello",
            "float": 3.14,
            "neg": -100,
            "big": 3000000000,
            "more_inner": [
                "str": "hello",
                "float": 3.14,
                "neg": -100,
                "big": 3000000000
            ]
        ],
        "bytes": data,
        "empty_list": [],
        "empty_dict": [:],
        "empty_object": {}
    ]

    convertAndPrintDictionary(smallDictionary)

    var desc = ac.local.ModelDesc()
    desc.inferenceType = "dummy"
    desc.name = "synthetic dummy"

    var a = createModel1(modelDescription: desc)
}
