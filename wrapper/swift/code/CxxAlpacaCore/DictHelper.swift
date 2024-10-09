// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//

// When we try to import CAlpacaCore which should expose C++ headers
// it tries to compile the module and starts with missing headers
// import CAlpacaCore

public func translateDictionaryToDict(_ dictionary: Dictionary<String, Any>) -> Int {
    print("translateDictionaryToDict")
    return 5
}

public func translateDictToDictionary(_ dict: Int) -> Dictionary<String, Any> {
    print("translateDictionaryToDict")
    return [:]
}
