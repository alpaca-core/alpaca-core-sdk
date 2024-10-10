// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CAlpacaCore

func test() -> ac.DictRoot {
    return ac.DictRoot.create()
}

public func translateDictionaryToDict(_ dictionary: Dictionary<String, Any>) -> Int {
    print("translateDictionaryToDict")
    return 5
}

public func translateDictToDictionary(_ dict: Int) -> Dictionary<String, Any> {
    print("translateDictionaryToDict")
    return [:]
}
