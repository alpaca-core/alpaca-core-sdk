import AlpacaCoreSwift
import XCTest

func progress(tag: String, progress: Float) {
    print("[\(tag)]Progress: \(progress)")
}

final class SwiftApiTests: XCTestCase {
    override class func setUp() {
        AlpacaCoreSwift.initSDK();
    }

    func testDummyInference() throws {
        print("Hello from e-Dummy.swift")

        var desc = AlpacaCoreSwift.ModelDesc()
        desc.inferenceType = "dummy"
        desc.name = "synthetic dummy"

        let dict = Dictionary<String, Any>()
        let model = try AlpacaCoreSwift.createModel(&desc, dict, progress);
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

    func testExpectedErrors() throws {
        var desc = AlpacaCoreSwift.ModelDesc()
        desc.inferenceType = "nope"
        desc.name = "synthetic dummy"
        let dict = Dictionary<String, Any>()

        XCTAssertThrowsError(try createModel(&desc, dict, progress)) { error in
            XCTAssertEqual(error as! ACError, ACError.invalidModelCreation("Unknown inference type: nope"))
        }

        desc.inferenceType = "dummy"
        let model = try createModel(&desc, dict, progress)
        XCTAssertThrowsError(try model.createInstance("nope", dict)) { error in
            XCTAssertEqual(error as! ACError, ACError.invalidInstanceCreation("dummy: unknown instance type: nope"))
        }

        let instance = try model.createInstance("general", dict)
        XCTAssertThrowsError(try instance.runOp("nope", dict, progress)) { error in
            XCTAssertEqual(error as! ACError, ACError.invalidRunOp("dummy: unknown op: nope"))
        }
    }
}
