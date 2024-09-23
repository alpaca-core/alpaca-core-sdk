// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api.example;

import com.alpacacore.api.ModelDesc;
import com.alpacacore.api.LocalProvider;
import com.alpacacore.api.Model;
import com.alpacacore.api.Instance;

public class LocalLlama {
    public static void main(String[] args) {
        System.out.println("Llama example");

        ModelDesc desc = new ModelDesc();
        desc.inferenceType = "llama.cpp";
        desc.assets = new ModelDesc.AssetInfo[1];
        desc.assets[0] = desc.new AssetInfo();
        desc.assets[0].path = "llama_model";
        desc.assets[0].tag = "model";
        desc.name = "Llama";

        LocalProvider.sandbox(desc);
    }
}
