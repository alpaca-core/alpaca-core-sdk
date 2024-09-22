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

        // LocalProvider.loadModel
    }
}
