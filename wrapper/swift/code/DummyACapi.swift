// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
public func test() {
    var data = Data() // "{\"pesho\": \"Hello, World!\"}".data(using: .utf8)!
    data.append("\"5\": 5".data(using: .utf8)!)
    let d = try? JSONSerialization.jsonObject(with: data)


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

    let model = ACModel()
    model.createInstance()

    print("kiiil")
}
