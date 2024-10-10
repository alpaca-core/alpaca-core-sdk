// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import CAlpacaCore

public func initSDK() {
    ac.initSDK()
}

func progress(_ progress: Float) {
    print("Progress: \(progress)")
}

public func createModel(_ desc: inout ModelDesc, _ params: Dictionary<String, Any>, _ _progress: (Float) -> Void) -> Model? {
    let paramsAsDict = translateDictionaryToDict(params)
    if let model = ac.createModel(&desc, paramsAsDict.getRef(), progress)
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

    public func runOp(_ op: String, _ params: Dictionary<String, Any>, _ _progress: (Float) -> Void) -> Dictionary<String, Any> {
        let paramsAsDict = translateDictionaryToDict(params)
        let resultDict = instance.runOp(std.string(op), paramsAsDict.getRef(), progress)
        return translateDictToDictionary(resultDict.getRef())
    }
}
