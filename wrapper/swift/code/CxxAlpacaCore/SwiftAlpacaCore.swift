public class AssetInfo {
    public var m_path: String = ""
    public var m_tag: String = ""

    public init() {
    }

    public init(_ path:String, _ tag:String) {
        m_path = path
        m_tag = tag
    }
}

public class ModelDesc {
    public var m_inferenceType: String = ""
    public var m_name: String = ""
    public var m_assets: [AssetInfo]

    public init() {
        self.m_assets = []
    }

    public init(_ inferenceType:String = "", _ name:String = "", _ assets:[AssetInfo] = []) {
        m_inferenceType = inferenceType
        m_name = name
        m_assets = assets
    }
}

public class Dict {
    public var m_dict: [String: Any] = [:]
    public init() {
    }
}
