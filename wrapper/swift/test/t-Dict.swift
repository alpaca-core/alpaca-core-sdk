import AlpacaCoreSwift
import XCTest

final class DictTests: XCTestCase {
    func testDictionaryToDict() throws {
        let pesho = String("john.appleseed@apple.com")
        XCTAssertEqual(pesho, "john.appleseed@apple.com", "sdads")

        let dictionary: [String: Any] = [
            "name": "Alice",
            "age": 28,
            "height": 1.6,
            "isMember": true,
            "scores": [88, 92, 79, "100"],
            "address": [
                "city": "Wonderland",
                "postalCode": "12345"
            ]
        ]

        let translated = AlpacaCoreSwift.translateDictionaryToDict(dictionary)
        let translatedRef = translated.getRef()
        XCTAssertEqual(String(translatedRef.atKey("name").getString()), dictionary["name"] as! String, "Couldn't convert String properly!")
        XCTAssertEqual(translatedRef.atKey("age").getInt(), dictionary["age"] as! Int, "Couldn't convert Int properly!")
        XCTAssertEqual(translatedRef.atKey("height").getDouble(), dictionary["height"] as! Double, "Couldn't convert Double properly!")
        XCTAssertEqual(translatedRef.atKey("isMember").getBool(), dictionary["isMember"] as! Bool, "Couldn't convert Bool properly!")
    }
}
