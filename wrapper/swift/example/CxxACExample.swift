// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import AlpacaCore
import CAlpacaCore


@main
struct CxxAlpacaCoreExample {
    static func main() {
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
        let acDict = ac.DictRoot.parse(std.string(jsonString))
        let dictRef = acDict.getRef()
        let nameRef = dictRef.atKey("name")
        print("Name: \(nameRef.getString())")

        let scoresRef = dictRef.atKey("scores")
        print("Name: \(ac.getDictTypeAsString(scoresRef.getType()))")
        var scoresRef2 = dictRef["scores"]
        print("Name: \(ac.getDictTypeAsString(scoresRef2.getType()))")

        let firstScore = scoresRef2[0]
        print("Name: \(ac.getDictTypeAsString(firstScore.getType()))")

        print("Value of score: \(firstScore.getUnsigned())")
        print("Value of score: \(dictRef["scores"][0].getUnsigned())")

        print("Values of score: \(scoresRef.dump())")

        var newElem = scoresRef2.addElement()
        newElem.setString("100")
        print("Values of score: \(scoresRef.dump())")
    }
}
