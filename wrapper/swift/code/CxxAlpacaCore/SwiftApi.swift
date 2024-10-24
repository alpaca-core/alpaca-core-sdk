// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CxxAlpacaCore

public enum ACError: Error, Equatable {
    case invalidModelCreation(String)
    case invalidInstanceCreation(String)
    case invalidRunOp(String)
}


public func initSDK() {
    AC.initSDK()
}

class CallbackWrapper {
    let completion: (String, Float) -> Void
    init(completion: @escaping (String, Float) -> Void) {
        self.completion = completion
    }

    public func getRawPointer() -> UnsafeMutableRawPointer {
        return UnsafeMutableRawPointer(Unmanaged.passRetained(self).toOpaque())
    }

    public func getProgressData() -> AC.ProgressCallbackData {
        return AC.ProgressCallbackData(m_cb: callObserver, m_context: getRawPointer())
    }
}

func callObserver(observer: UnsafeMutableRawPointer, tag: std.string, progress: Float) {
    let wrapper = Unmanaged<CallbackWrapper>.fromOpaque(observer).takeUnretainedValue()
    wrapper.completion(String(tag), progress)
}

public func createModel(_ desc: inout ModelDesc, _ params: Dictionary<String, Any>,
    _ progress: @escaping (String, Float) -> Void) throws -> Model {
    let paramsAsDict = try translateDictionaryToDict(params)
    let wrapper = CallbackWrapper(completion: progress)

    var result = AC.createModel(&desc, paramsAsDict.getRef(), wrapper.getProgressData())
    if result.hasError() {
        throw ACError.invalidModelCreation(String(result.error()))
    }
    return Model(result.consumeValue())
}

public class Model {
    var model: AC.Model

    init(_ model: AC.Model) {
        self.model = model
    }

    public func createInstance(_ name: String, _ params: Dictionary<String, Any>) throws -> Instance {
        let paramsAsDict = try translateDictionaryToDict(params)
        var result = model.createInstance(std.string(name), paramsAsDict.getRef())
        if result.hasError() {
            throw ACError.invalidInstanceCreation(String(result.error()))
        }
        return Instance(result.consumeValue())
    }
}

public class Instance {
    var instance: AC.Instance

    init(_ instance: AC.Instance) {
        self.instance = instance
    }

    public func runOp(_ op: String, _ params: Dictionary<String, Any>,
            _ progress: @escaping (String, Float) -> Void) throws -> Dictionary<String, Any> {
        let paramsAsDict = try translateDictionaryToDict(params)
        let wrapper = CallbackWrapper(completion: progress)

        var result = instance.runOp(std.string(op), paramsAsDict.getRef(), wrapper.getProgressData())
        if result.hasError() {
            throw ACError.invalidRunOp(String(result.error()))
        }
        return try translateDictToDictionary(result.consumeValue().getRef())
    }
}
