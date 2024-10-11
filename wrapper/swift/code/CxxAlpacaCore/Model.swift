// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CAlpacaCore

public func initSDK() {
    ac.initSDK()
}

class CallbackWrapper {
    let completion: (Float) -> Void
    init(completion: @escaping (Float) -> Void) {
        self.completion = completion
    }

    public func getRawPointer() -> UnsafeMutableRawPointer {
        return UnsafeMutableRawPointer(Unmanaged.passRetained(self).toOpaque())
    }
}

func callObserver(observer: UnsafeMutableRawPointer, progress: Float) {
    let wrapper = Unmanaged<CallbackWrapper>.fromOpaque(observer).takeUnretainedValue()
    wrapper.completion(progress)
}

public func createModel(_ desc: inout ModelDesc, _ params: Dictionary<String, Any>, _ _progress: @escaping (Float) -> Void) -> Model? {
    let paramsAsDict = translateDictionaryToDict(params)
    let wrapper = CallbackWrapper(completion: _progress)

    if let model = ac.createModel(&desc, paramsAsDict.getRef(), callObserver, wrapper.getRawPointer())
    {
        return Model(model)
    }
    return nil
}

public class Model {
    var model: ac.Model

    init(_ model: ac.Model) {
        self.model = model
    }

    public func createInstance(_ name: String, _ params: Dictionary<String, Any>) -> Instance {
        let paramsAsDict = translateDictionaryToDict(params)
        return Instance(model.createInstance(std.string(name), paramsAsDict.getRef()))
    }
}

public class Instance {
    var instance: ac.Instance

    init(_ instance: ac.Instance) {
        self.instance = instance
    }

    public func runOp(_ op: String, _ params: Dictionary<String, Any>, _ _progress: @escaping (Float) -> Void) -> Dictionary<String, Any> {
        let paramsAsDict = translateDictionaryToDict(params)
        let wrapper = CallbackWrapper(completion: _progress)

        let resultDict = instance.runOp(std.string(op), paramsAsDict.getRef(), callObserver, wrapper.getRawPointer())
        return translateDictToDictionary(resultDict.getRef())
    }
}
