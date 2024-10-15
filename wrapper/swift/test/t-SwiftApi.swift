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
        let model = AlpacaCoreSwift.createModel(&desc, dict, {
            (progress: Float) in
            print("Progress: \(progress)")
        });
        XCTAssertNotNil(model)

        let instance = model!.createInstance("general", dict);
        XCTAssertNotNil(instance)

        let params = [
            "input": ["a", "b"]
        ]

        let result = instance.runOp("run", params, {
            (progress: Float) in
            print("Progress: \(progress)")
        });

        XCTAssertEqual(result["result"] as! String, "a one b two", "Incorrect result!")
    }
}
