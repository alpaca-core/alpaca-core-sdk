// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import CxxAlpacaCore

@main
struct CxxAlpacaCoreExample {
    static func main() {
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
    }
}
