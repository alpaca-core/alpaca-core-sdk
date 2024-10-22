// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CAlpacaCore

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

public func createModel(_ desc: inout ModelDesc, _ params: Dictionary<String, Any>, _ _progress: @escaping (String, Float) -> Void) -> Model? {
    let paramsAsDict = translateDictionaryToDict(params)
    let wrapper = CallbackWrapper(completion: _progress)

    let model = AC.createModel(&desc, paramsAsDict.getRef(), wrapper.getProgressData())
    return Model(model)
}

public class Model {
    var model: AC.Model

    init(_ model: AC.Model) {
        self.model = model
    }

    public func createInstance(_ name: String, _ params: Dictionary<String, Any>) -> Instance {
        let paramsAsDict = translateDictionaryToDict(params)
        return Instance(model.createInstance(std.string(name), paramsAsDict.getRef()))
    }
}

public class Instance {
    var instance: AC.Instance

    init(_ instance: AC.Instance) {
        self.instance = instance
    }

    public func runOp(_ op: String, _ params: Dictionary<String, Any>, _ _progress: @escaping (String, Float) -> Void) -> Dictionary<String, Any> {
        let paramsAsDict = translateDictionaryToDict(params)
        let wrapper = CallbackWrapper(completion: _progress)

        let resultDict = instance.runOp(std.string(op), paramsAsDict.getRef(), wrapper.getProgressData())
        return translateDictToDictionary(resultDict.getRef())
    }
}
