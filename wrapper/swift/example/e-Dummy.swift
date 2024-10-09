// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
import Foundation
import CAlpacaCore
import AlpacaCore

func progress(_ progress: Float) {
    print("Progress: \(progress)")
}

@main
struct DummyExample {
    static func main() {
        print("Hello from e-Dummy.swift")

        ac.initSDK();

        var desc = AlpacaCore.ModelDesc()
        desc.inferenceType = "dummy"
        desc.name = "synthetic dummy"

        let dict = ac.DictRoot.create()
        let model = ac.createModel(&desc, dict.getRef(), progress)!;
        let instance = model.createInstance("general", dict.getRef());

        let dictOp = ac.DictRoot.create()
        let inputRef = dictOp.getRef().addChild("input")
        var objRef = inputRef.addElement()
        objRef.setString("a")
        var objRef2 = inputRef.addElement()
        objRef2.setString("b")

        let result = instance.runOp("run", dictOp.getRef(), progress);
        print("Result \(result.getRef().dump())")
    }
}
