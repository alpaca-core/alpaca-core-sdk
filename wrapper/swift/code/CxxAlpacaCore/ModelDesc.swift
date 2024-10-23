public class AssetInfo {
    public var path: String = ""
    public var tag: String = ""

    public init() {
    }

    public init(_ path:String, _ tag:String) {
        self.path = path
        self.tag = tag
    }
}

public class ModelDesc {
    public var inferenceType: String = ""
    public var name: String = ""
    public var assets: [AssetInfo]

    public init() {
        self.assets = []
    }

    public init(_ inferenceType:String = "", _ name:String = "", _ assets:[AssetInfo] = []) {
        self.inferenceType = inferenceType
        self.name = name
        self.assets = assets
    }
}
