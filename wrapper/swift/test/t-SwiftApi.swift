import AlpacaCoreSwift
import XCTest

final class SwiftApiTests: XCTestCase {
    func testModelCreation() throws {
        print("Hello from e-Dummy.swift")

        AlpacaCoreSwift.initSDK();

        var desc = AlpacaCoreSwift.ModelDesc()
        desc.inferenceType = "dummy"
        desc.name = "synthetic dummy"

        let dict = Dictionary<String, Any>()
        let model = try AlpacaCoreSwift.createModel(&desc, dict, {
            (tag: String, progress: Float) in
            print("[\(tag)]Progress: \(progress)")
        });
        XCTAssertNotNil(model)

        let instance = try model.createInstance("general", dict);
        XCTAssertNotNil(instance)

        let params = [
            "input": ["a", "b"]
        ]

        let result = try instance.runOp("run", params, {
            (tag: String, progress: Float) in
            print("[\(tag)]Progress: \(progress)")
        });

        XCTAssertEqual(result["result"] as! String, "a one b two", "Incorrect result!")
    }
}
