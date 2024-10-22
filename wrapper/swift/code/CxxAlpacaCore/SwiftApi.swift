// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CAlpacaCore

public enum ACError: Error {
    case invalidModelCreation(String)
    case invalidInstanceCreation(String)
    case invalidRunOp(String)
}


public func initSDK() {
    AC.initSDK()
}

public func loadWavF32Mono(_ filePath: String) -> [Float] {
    let pcmf32 = AC.loadWavF32Mono(std.string(filePath))
    return Array<Float>(pcmf32)
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

func callObserver(observer: UnsafeMutableRawPointer, tag: UnsafePointer<Int8>, progress: Float) {
    let wrapper = Unmanaged<CallbackWrapper>.fromOpaque(observer).takeUnretainedValue()
    wrapper.completion(String(cString: tag), progress)
}

public func createModel(_ desc: inout ModelDesc, _ params: Dictionary<String, Any>,
    _ progress: @escaping (String, Float) -> Void) throws -> Model {
    let paramsAsDict = translateDictionaryToDict(params)
    let wrapper = CallbackWrapper(completion: progress)

    let result = AC.createModel(&desc, paramsAsDict.getRef(), wrapper.getProgressData())
    if result.hasError() {
        throw ACError.invalidModelCreation(String(result.error()))
    }
    return Model(result.value())
}

public class Model {
    var model: AC.Model

    init(_ model: AC.Model) {
        self.model = model
    }

    public func createInstance(_ name: String, _ params: Dictionary<String, Any>) throws -> Instance {
        let paramsAsDict = translateDictionaryToDict(params)
        let result = model.createInstance(std.string(name), paramsAsDict.getRef())
        if result.hasError() {
            throw ACError.invalidInstanceCreation(String(result.error()))
        }
        return Instance(result.value())
    }
}

public class Instance {
    var instance: AC.Instance

    init(_ instance: AC.Instance) {
        self.instance = instance
    }

    public func runOp(_ op: String, _ params: Dictionary<String, Any>,
            _ progress: @escaping (String, Float) -> Void) throws -> Dictionary<String, Any> {
        let paramsAsDict = translateDictionaryToDict(params)
        let wrapper = CallbackWrapper(completion: progress)

        let result = instance.runOp(std.string(op), paramsAsDict.getRef(), wrapper.getProgressData())
        if result.hasError() {
            throw ACError.invalidRunOp(String(result.error()))
        }
        return translateDictToDictionary(result.value().getRef())
    }
}
