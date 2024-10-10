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

public func createModel(_ desc: inout ModelDesc, _ params: ac.DictRef, _ _progress: (Float) -> Void) -> Model? {
    if let model = ac.createModel(&desc, params, progress)
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

    public func createInstance(_ name: String, _ params: ac.DictRef) -> Instance {
        return Instance(model.createInstance(std.string(name), params))
    }

}

public class Instance {
    var instance: ac.Instance

    init(_ instance: ac.Instance) {
        self.instance = instance
    }

    public func runOp(_ op: String, _ params: ac.DictRef, _ _progress: (Float) -> Void) -> ac.DictRoot {
        return instance.runOp(std.string(op), params, progress)
    }
}
