public class AssetInfo {
    public var m_path: String = ""
    public var m_tag: String = ""

    public init() {
    }

    public init(_path:String, _tag:String) {
        m_path = _path
        m_tag = _tag
    }
}

public class ModelDesc {
    public var m_inferenceType: String = ""
    public var m_name: String = ""
    public var m_assets: [AssetInfo]

    public init() {
        self.m_assets = []
    }

    public init(_inferenceType:String = "", _name:String = "", _assets:[AssetInfo] = []) {
        m_inferenceType = _inferenceType
        m_name = _name
        m_assets = _assets
    }
}

public class Dict {
    public var m_dict: [String: Any] = [:]
    public init() {
    }
}
