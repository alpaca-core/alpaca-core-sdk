import AlpacaCoreSwift
import XCTest

final class DictTests: XCTestCase {
    func testDictionaryToDict() throws {
        let dictionary: [String: Any] = [
            "name": "Alice",
            "age": 28,
            "height": 1.6,
            "isMember": true,
            "scores": [88, true, 1.8, "100", "Hello, World!".data(using: .utf8)!],
            "address": [
                "city": "Wonderland",
                "postalCode": 12345
            ],
            "binData": "Hello, World!".data(using: .utf8)!
        ]

        let translated = try AlpacaCoreSwift.translateDictionaryToDict(dictionary)
        let translatedRef = translated.getRef()

        XCTAssertEqual(String(translatedRef.atKey("name").getString()),
            dictionary["name"] as! String, "Couldn't convert String properly!")
        XCTAssertEqual(translatedRef.atKey("age").getInt(),
             dictionary["age"] as! Int, "Couldn't convert Int properly!")
        XCTAssertEqual(translatedRef.atKey("height").getDouble(),
            dictionary["height"] as! Double, "Couldn't convert Double properly!")
        XCTAssertEqual(translatedRef.atKey("isMember").getBool(),
            dictionary["isMember"] as! Bool, "Couldn't convert Bool properly!")

        let scoresArr = translatedRef.atKey("scores")
        let origScores = dictionary["scores"] as! [Any]

        XCTAssertEqual(scoresArr.getSize(), origScores.count, "Arrays have different count!")
        XCTAssertEqual(scoresArr.atIndex(0).getInt(), origScores[0] as! Int,
            "Couldn't convert Int in Array properly!")
        XCTAssertEqual(scoresArr.atIndex(1).getBool(), origScores[1] as! Bool,
            "Couldn't convert Bool in Array properly!")
        XCTAssertEqual(scoresArr.atIndex(2).getDouble(), origScores[2] as! Double,
            "Couldn't convert Double in Array properly!")
        XCTAssertEqual(String(scoresArr.atIndex(3).getString()), origScores[3] as! String,
             "Couldn't convert String in Array properly!")

        let acArrBinData = scoresArr.atIndex(4).getBinary()
        var dataFromDict = Data(bytes: acArrBinData.data, count: acArrBinData.size)
        var strFromData = String(data: dataFromDict, encoding: .utf8)!

        var strFromOrigData = String(data: origScores[4] as! Data, encoding: .utf8)!

        XCTAssertEqual(strFromData, strFromOrigData, "Couldn't convert binary data in Array properly!")

        let addressObj = translatedRef.atKey("address")
        let origAddress = dictionary["address"] as! [String: Any]

        XCTAssertEqual(String(addressObj.atKey("city").getString()),
            origAddress["city"] as! String, "Couldn't convert String from Object properly!")
        XCTAssertEqual(addressObj.atKey("postalCode").getInt(),
            origAddress["postalCode"] as! Int, "Couldn't convert Int from Object properly!")

        let acBinData = translatedRef.atKey("binData").getBinary()
        dataFromDict = Data(bytes: acBinData.data, count: acBinData.size)
        strFromData = String(data: dataFromDict, encoding: .utf8)!

        strFromOrigData = String(data: dictionary["binData"] as! Data, encoding: .utf8)!

        XCTAssertEqual(strFromData, strFromOrigData, "Couldn't convert binary data properly!")
    }

    func testDictToDictionary() throws {
        let dictionary: [String: Any] = [
            "name": "Alice",
            "age": 28,
            "height": 1.6,
            "isMember": true,
            "scores": [88, true, 1.8, "100", "Hello, World!".data(using: .utf8)!],
            "address": [
                "city": "Wonderland",
                "postalCode": 12345
            ],
            "binData": "Hello, World!".data(using: .utf8)!
        ]

        let translated = try AlpacaCoreSwift.translateDictionaryToDict(dictionary)
        let newDictionary = try AlpacaCoreSwift.translateDictToDictionary(translated.getRef())

        XCTAssertEqual(newDictionary["name"] as? String, dictionary["name"] as? String, "Couldn't convert String properly!")
        XCTAssertEqual(newDictionary["age"] as? Int, dictionary["age"] as? Int, "Couldn't convert Int properly!")
        XCTAssertEqual(newDictionary["height"] as? Double, dictionary["height"] as? Double, "Couldn't convert Double properly!")
        XCTAssertEqual(newDictionary["isMember"] as? Bool, dictionary["isMember"] as? Bool, "Couldn't convert Bool properly!")

        let scoresArray = newDictionary["scores"] as? [Any]
        let originalScores = dictionary["scores"] as! [Any]

        XCTAssertNotNil(scoresArray, "Array 'scores' is nil!")
        XCTAssertEqual(scoresArray?.count, originalScores.count, "Arrays have different count!")

        XCTAssertEqual(scoresArray?[0] as? Int, originalScores[0] as? Int, "Couldn't convert Int in Array properly!")
        XCTAssertEqual(scoresArray?[1] as? Bool, originalScores[1] as? Bool, "Couldn't convert Bool in Array properly!")
        XCTAssertEqual(scoresArray?[2] as? Double, originalScores[2] as? Double, "Couldn't convert Double in Array properly!")
        XCTAssertEqual(scoresArray?[3] as? String, originalScores[3] as? String, "Couldn't convert String in Array properly!")

        let dataFromArray = scoresArray?[4] as? Data
        let originalData = originalScores[4] as! Data
        XCTAssertNotNil(dataFromArray, "Binary data in Array is nil!")
        XCTAssertEqual(dataFromArray, originalData, "Couldn't convert binary data in Array properly!")

        let addressDict = newDictionary["address"] as? [String: Any]
        let originalAddress = dictionary["address"] as! [String: Any]

        XCTAssertNotNil(addressDict, "Address dictionary is nil!")
        XCTAssertEqual(addressDict?["city"] as? String, originalAddress["city"] as? String,
            "Couldn't convert String from nested dictionary properly!")
        XCTAssertEqual(addressDict?["postalCode"] as? Int, originalAddress["postalCode"] as? Int,
            "Couldn't convert Int from nested dictionary properly!")

        let binaryData = newDictionary["binData"] as? Data
        let originalBinData = dictionary["binData"] as! Data

        XCTAssertNotNil(binaryData, "Binary data is nil!")
        XCTAssertEqual(binaryData, originalBinData, "Couldn't convert binary data properly!")
    }

    func testExpectedErrors() throws {
        class Point {
            var x: Int
            var y: Int

            init(x: Int, y: Int) {
                self.x = x
                self.y = y
            }
        }
        let dictionary: [String: Any] = [
            "name": "Alice",
            "age": 28,
            "height": 1.6,
            "point": Point(x: 1, y: 2)
        ]
        XCTAssertThrowsError(try AlpacaCoreSwift.translateDictionaryToDict(dictionary)) { error in
            XCTAssertEqual(error as! DictConvertError, DictConvertError.invalidType("Invalid type (Point) for dictionary value"))
        }
    }
}
