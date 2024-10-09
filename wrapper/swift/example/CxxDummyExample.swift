// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import CxxACLocal
import CxxDummyInference
import AlpacaCore
import CAlpacaCore

func progress(tag: Optional<String>, progress: Float) -> Bool {
    print("[\(tag!)] progress: \(progress)\n")

    return true
}

@main
struct CxxDummyExample {
    static func main() {

    }
}
