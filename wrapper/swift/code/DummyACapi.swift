// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
public func test() {
    let smallDictionary: [String: Any] = [
        "name": "Alice",
        "age": 28,
        "isMember": true,
        "address": [
            "street": "123 Apple St",
            "city": "Cupertino",
            "zip": "95014"
        ],
        "scores": [88, 92, 79]
    ]

    print(smallDictionary)

    let dictWrapper = DictionaryWrapper()
    print(dictWrapper.toJSONString(fromDict: smallDictionary))
}
