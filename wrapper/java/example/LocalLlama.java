// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api.example;

import com.alpacacore.api.ModelDesc;
import com.alpacacore.api.LocalProvider;
import com.alpacacore.api.Model;
import com.alpacacore.api.Instance;

import java.util.Map;

import java.util.concurrent.CountDownLatch;

public class LocalLlama {
    static final class Box<T> {
        T value;
    }

    public static void main(String[] args) throws Exception {
        System.out.println("Llama example");

        ModelDesc desc = new ModelDesc();
        desc.inferenceType = "dummy";
        desc.name = "synthetic dummy";

        Box<Model> modelBox = new Box<Model>();
        CountDownLatch loadModelLatch = new CountDownLatch(1);
        LocalProvider.loadModel(desc, null, new LocalProvider.LoadModelCallback() {
            @Override
            public void onComplete(Model result) {
                System.out.println("Model loaded");
                modelBox.value = result;
                loadModelLatch.countDown();
            }

            @Override
            public void onError(String error) {
                System.out.println("Model load error: " + error);
                loadModelLatch.countDown();
            }

            @Override
            public void onProgress(String tag, float progress) {
                System.out.println("Model load progress: " + tag + " " + progress);
            }
        });

        loadModelLatch.await();

        Model model = modelBox.value;
        if (model == null) {
            System.out.println("Model load failed");
            return;
        }

        Box<Instance> instanceBox = new Box<Instance>();
        CountDownLatch createInstanceLatch = new CountDownLatch(1);
        model.createInstance("general", null, new Model.CreateInstanceCallback() {
            @Override
            public void onComplete(Instance result) {
                System.out.println("Instance created");
                instanceBox.value = result;
                createInstanceLatch.countDown();
            }

            @Override
            public void onError(String error) {
                System.out.println("Instance create error: " + error);
                createInstanceLatch.countDown();
            }
        });

        createInstanceLatch.await();

        Instance instance = instanceBox.value;
        if (instance == null) {
            System.out.println("Instance create failed");
            return;
        }

        Box<String> opResultBox = new Box<String>();
        CountDownLatch opLatch = new CountDownLatch(1);
        instance.runOp("run", Map.of("input", new String[]{"a", "b", "c"}), new Instance.OpCallback() {
            @Override
            public void onComplete() {
                System.out.println("\nOp complete");
                opLatch.countDown();
            }

            @Override
            public void onError(String error) {
                System.out.println("\nOp error: " + error);
                opLatch.countDown();
            }

            @Override
            public void onProgress(String tag, float progress) {
                System.out.println("\nOp progress: " + tag + " " + progress);
            }

            @Override
            public void onStream(Object data) {
                Map hm = (Map)data;
                opResultBox.value = (String)hm.get("result");
            }
        });

        opLatch.await();

        String opResult = opResultBox.value;

        System.out.println("Op result: " + opResult);
    }
}
