public class SwiftStr {
    var str: String

    public init() {
        str = ""
    }

    public init(_ s:String) {
        str = s
    }

    public func parseJson(_ jsonString: String) {
        str = jsonString
    }

    public func dump() -> String {
        return str
    }
}
