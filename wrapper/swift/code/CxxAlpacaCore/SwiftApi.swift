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

class ProgressCallbackWrapper {
    let cb: Optional<(String, Float) -> Void>
    init(_ cb: Optional<(String, Float) -> Void>) {
        self.cb = cb
    }

    public func getRawPointer() -> UnsafeMutableRawPointer {
        return UnsafeMutableRawPointer(Unmanaged.passRetained(self).toOpaque())
    }

    public func releaseRawPointer(_ pointer: Optional<UnsafeMutableRawPointer>) {
        if pointer != nil {
            let unmanaged = Unmanaged<ProgressCallbackWrapper>.fromOpaque(pointer!)
            unmanaged.release()
        }
    }
}

func callObserver(observer: UnsafeMutableRawPointer, tag: std.string, progress: Float) {
    let wrapper = Unmanaged<ProgressCallbackWrapper>.fromOpaque(observer).takeUnretainedValue()
    if wrapper.cb != nil {
        wrapper.cb!(String(tag), progress)
    }
}

public func createModel(_ desc: inout ModelDesc, _ params: Dictionary<String, Any>,
                        _ progress: Optional<(String, Float) -> Void> = nil) throws -> Model {
    let paramsAsDict = try translateDictionaryToDict(params)
    let wrapper = ProgressCallbackWrapper(progress)
    var cbData: AC.ProgressCallbackData = AC.ProgressCallbackData()
    if progress != nil {
        cbData = AC.ProgressCallbackData(m_cb: callObserver, m_context: wrapper.getRawPointer())
    }

    var result = AC.createModel(&desc, paramsAsDict.getRef(), cbData)
    wrapper.releaseRawPointer(cbData.m_context)
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
                      _ progress: Optional<(String, Float) -> Void> = nil) throws -> Dictionary<String, Any> {
        let paramsAsDict = try translateDictionaryToDict(params)

        let wrapper = ProgressCallbackWrapper(progress)
        var cbData: AC.ProgressCallbackData = AC.ProgressCallbackData()
        if progress != nil {
            cbData = AC.ProgressCallbackData(m_cb: callObserver, m_context: wrapper.getRawPointer())
        }

        var result = instance.runOp(std.string(op), paramsAsDict.getRef(), cbData)
        wrapper.releaseRawPointer(cbData.m_context)

        if result.hasError() {
            throw ACError.invalidRunOp(String(result.error()))
        }
        return try translateDictToDictionary(result.consumeValue().getRef())
    }
}
