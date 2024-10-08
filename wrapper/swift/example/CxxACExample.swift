// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import AlpacaCore
import CxxAlpacaCoreDict


@main
struct CxxAlpacaCoreExample {
    static func main() {
        // Create a string object from Swift's Alpaca library
        let acd = AlpacaCore.SwiftStr()
        acd.parseJson("Hello, World!")
        print("AC String: \(acd.dump())")

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
        print("Dict type: \n\t \(ac.getDictTypeAsString(acDict))")

        // Check how we're going to handle errors
        let k = acDict.getDictAt("kur")
        let _ = k.getInt()
        // end check

        let name = acDict.getDictAt("name")
        print("Type: \(ac.getDictTypeAsString(name)) - val; \(name.getString())")

        let age = acDict.getDictAt("age")
        print("Type: \(ac.getDictTypeAsString(age)) - val; \(age.getInt())")

        let h = acDict.getDictAt("height")
        print("Type: \(ac.getDictTypeAsString(h)) - val; \(h.getDouble())")

        let m = acDict.getDictAt("isMember")
        print("Type: \(ac.getDictTypeAsString(m)) - val; \(m.getBool())")

        let arrDict = acDict.getDictAt("scores")
        print("Type: \(ac.getDictTypeAsString(arrDict)) - \(type(of: arrDict))")

        let arrVec = arrDict.getArray()
        let arr1 = arrVec[0]
        print("Type: \(ac.getDictTypeAsString(arr1)) - val \(arr1.getInt()) [\(type(of: arr1.getInt()))]")

        let arr2 = arrVec[1]
        print("Type: \(ac.getDictTypeAsString(arr2)) - val \(arr2.getInt()) [\(type(of: arr2.getInt()))]")

        let obj = acDict.getDictAt("address")
        print("Type: \(ac.getDictTypeAsString(obj))")

        let city = obj.getDictAt("city")
        print("Type: \(ac.getDictTypeAsString(city)) - val; \(city.getString())")

        let code = obj.getDictAt("postalCode")
        print("Type: \(ac.getDictTypeAsString(code)) - val; \(code.getString())")

        var newDict = ac.SwiftACDict()
        newDict.setString("pesho")
        var anotherDict = ac.SwiftACDict()
        anotherDict.setDictAt("name", newDict)

        print("Dict as string: \n\t \(anotherDict.dump())")
        newDict.setInt(33)
        anotherDict.setDictAt("age", newDict)

        newDict.setUnsigned(444444444)
        anotherDict.setDictAt("weight", newDict)

        newDict.setDouble(1.89)
        anotherDict.setDictAt("height", newDict)

        newDict.setBool(true)
        anotherDict.setDictAt("isFunny", newDict)

        let data = "pesho".data(using: .utf8)
        data!.withUnsafeBytes { (rawBufferPointer: UnsafeRawBufferPointer) in
            let ptr = rawBufferPointer.bindMemory(to: UInt8.self).baseAddress!
            let size = data?.count

            newDict.setBinary(ptr, UInt32(size!))
        }

        anotherDict.setDictAt("binary", newDict)

        print("Dict as string: \n\t \(anotherDict.dump())")
    }
}
