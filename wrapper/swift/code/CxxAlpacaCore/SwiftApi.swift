// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CAlpacaCore

public func initSDK() {
    ac.swift.initSDK()
}

public func loadWavF32Mono(_ filePath: String) -> [Float] {
    let pcmf32 = ac.swift.loadWavF32Mono(std.string(filePath))
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

    public func getProgressData() -> ac.swift.ProgressCallbackData {
        return ac.swift.ProgressCallbackData(m_cb: callObserver, m_context: getRawPointer())
    }
}

func callObserver(observer: UnsafeMutableRawPointer, tag: UnsafePointer<Int8>, progress: Float) {
    let wrapper = Unmanaged<CallbackWrapper>.fromOpaque(observer).takeUnretainedValue()
    wrapper.completion(String(cString: tag), progress)
}

public func createModel(_ desc: inout ModelDesc, _ params: Dictionary<String, Any>, _ _progress: @escaping (String, Float) -> Void) -> Model? {
    let paramsAsDict = translateDictionaryToDict(params)
    let wrapper = CallbackWrapper(completion: _progress)

    if let model = ac.swift.createModel(&desc, paramsAsDict.getRef(), wrapper.getProgressData())
    {
        return Model(model)
    }
    return nil
}

public class Model {
    var model: ac.swift.Model

    init(_ model: ac.swift.Model) {
        self.model = model
    }

    public func createInstance(_ name: String, _ params: Dictionary<String, Any>) -> Instance {
        let paramsAsDict = translateDictionaryToDict(params)
        return Instance(model.createInstance(std.string(name), paramsAsDict.getRef()))
    }
}

public class Instance {
    var instance: ac.swift.Instance

    init(_ instance: ac.swift.Instance) {
        self.instance = instance
    }

    public func runOp(_ op: String, _ params: Dictionary<String, Any>, _ _progress: @escaping (String, Float) -> Void) -> Dictionary<String, Any> {
        let paramsAsDict = translateDictionaryToDict(params)
        let wrapper = CallbackWrapper(completion: _progress)

        let resultDict = instance.runOp(std.string(op), paramsAsDict.getRef(), wrapper.getProgressData())
        return translateDictToDictionary(resultDict.getRef())
    }
}
