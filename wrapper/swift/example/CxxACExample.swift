// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import AlpacaCoreSwift
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
        let scoresRef2 = dictRef["scores"]
        print("Name: \(ac.getDictTypeAsString(scoresRef2.getType()))")

        let firstScore = scoresRef2[0]
        print("Name: \(ac.getDictTypeAsString(firstScore.getType()))")

        print("Value of score: \(firstScore.getUnsigned())")
        print("Value of score: \(dictRef["scores"][0].getUnsigned())")

        print("Values of score: \(scoresRef.dump())")

        var newElem = scoresRef2.addElement()
        newElem.setString("100")
        print("Values of score: \(scoresRef.dump())")

        var child = dictRef.addChild("phoneNumber")
        child.setString("123-456-7890")

        var binData = dictRef.addChild("binData")
        let data = "Hello, World!".data(using: .utf8)!
        data.withUnsafeBytes { (ptr: UnsafeRawBufferPointer) in
            binData.setBinary(ptr.baseAddress!, data.count)
        }

        print("Values of binData: \(binData.dump()) - type \(ac.getDictTypeAsString(binData.getType()))")

        let binaryDataRef = dictRef["binData"]
        print("Values of binData: \(binaryDataRef.dump()) - type \(ac.getDictTypeAsString(binaryDataRef.getType()))")

        let binaryData = binaryDataRef.getBinary()
        print("bin Data \(binaryData)")
        let dataFromDict = Data(bytes: binaryData.data, count: binaryData.size)
        let strFromData = String(data: dataFromDict, encoding: .utf8)!
        print("Strinf from Binary data: \(strFromData)")

        let swiftDict = translateDictToDictionary(dictRef)
        print("Swift Dict: \(swiftDict)")

        let swiftDictTranslated = translateDictionaryToDict(swiftDict)
        print("Translated: \(swiftDictTranslated.getRef().dump())")

        let dictionary: [String: Any] = [
            "name": "Alice",
            "age": 28,
            "height": 1.6,
            "isMember": true,
            "scores": [88, 92, 79, "100"],
            "address": [
                "city": "Wonderland",
                "postalCode": "12345"
            ],
            "phoneNumber": "123-456-7890"
        ]

        print("Swift Orig Dict: \(dictionary)")

        let translated = translateDictionaryToDict(dictionary)
        print("Translated: \(translated.getRef().dump())")
    }
}
