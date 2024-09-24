// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

import org.junit.Test;
import static org.junit.Assert.*;

import java.util.concurrent.CountDownLatch;

final class Result<T> {
    public T value;
    public String error;
}

public class TestLocalProvider {
    class SyncHelper {
        Result<Model> model = new Result<Model>();
        String modelProgressTag;

        public void loadModel(ModelDesc desc) throws Exception {
            CountDownLatch latch = new CountDownLatch(1);
            LocalProvider.loadModel(desc, null, new LocalProvider.LoadModelCallback() {
                @Override
                public void onComplete(Model result) {
                    model.value = result;
                    latch.countDown();
                }

                @Override
                public void onError(String error) {
                    model.error = error;
                    latch.countDown();
                }

                @Override
                public void onProgress(String tag, float progress) {
                    modelProgressTag = tag;
                }
            });
            latch.await();
        }
    }

    @Test
    public void errorNoInferenceProvider() throws Exception {
        SyncHelper h = new SyncHelper();

        ModelDesc desc = new ModelDesc("nope", null, null);
        h.loadModel(desc);
        assertEquals("Unknown model type", h.model.error);
        assertNull(h.modelProgressTag);
    }

    @Test
    public void haveLlamaInference() throws Exception {
        SyncHelper h = new SyncHelper();

        ModelDesc desc = new ModelDesc("llama.cpp", null, null);
        h.loadModel(desc);
        assertEquals("llama: expected exactly one local asset", h.model.error);
        assertNull(h.modelProgressTag);
    }

    @Test
    public void haveWhisperInference() throws Exception {
        SyncHelper h = new SyncHelper();

        ModelDesc desc = new ModelDesc("whisper.cpp", null, null);
        h.loadModel(desc);
        assertEquals("whisper: expected exactly one local asset", h.model.error);
        assertNull(h.modelProgressTag);
    }

    @Test
    public void dummyBadModel() throws Exception {
        SyncHelper h = new SyncHelper();

        ModelDesc.AssetInfo[] assets = {new ModelDesc.AssetInfo()};
        assets[0].path = "nope";
        ModelDesc desc = new ModelDesc("dummy", assets, "test");
        h.loadModel(desc);
        assertEquals("Failed to open file: nope", h.model.error);
        assertEquals("nope", h.modelProgressTag);
    }
}
