// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.example;

import com.alpacacore.AlpacaCore;
import com.alpacacore.ModelDesc;
import com.alpacacore.Model;
import com.alpacacore.Instance;

import java.util.Map;


public class LocalDummyExample {
    public static void main(String[] args) throws Exception {
        System.out.println("Dummy example");

        ModelDesc desc = new ModelDesc();
        desc.inferenceType = "dummy";
        desc.name = "synthetic dummy";

        Model model = AlpacaCore.createModel(desc, null, null);

        Instance instance = model.createInstance("general", null);

        Map result = (Map)instance.runOp("run", Map.of("input", new String[]{"a", "b", "c"}), null);

        System.out.println(result);

        AlpacaCore.releaseInstance(instance);
        AlpacaCore.releaseModel(model);
    }
}
