// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

import org.junit.Test;
import static org.junit.Assert.*;

import java.util.Map;
import java.util.concurrent.CountDownLatch;

final class Result<T> {
    public T value;
    public String error;
}

public class TestLocalProvider {
    static final class SyncHelper {
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

        Result<Instance> instance = new Result<Instance>();
        public void createInstance(String type, Object params) throws Exception {
            CountDownLatch latch = new CountDownLatch(1);
            model.value.createInstance(type, params, new Model.CreateInstanceCallback() {
                @Override
                public void onComplete(Instance result) {
                    instance.value = result;
                    latch.countDown();
                }

                @Override
                public void onError(String error) {
                    instance.error = error;
                    latch.countDown();
                }
            });
            latch.await();
        }

        Result<Object> instantOp = new Result<Object>();
        public void runInstantOp(String type, Object params) throws Exception {
            CountDownLatch latch = new CountDownLatch(1);
            instance.value.runOp(type, params, new Instance.OpCallback() {
                @Override
                public void onComplete() {
                    latch.countDown();
                }

                @Override
                public void onError(String error) {
                    instantOp.error = error;
                    latch.countDown();
                }

                @Override
                public void onProgress(String tag, float progress) {}

                @Override
                public void onStream(Object data) {
                    instantOp.value = data;
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

    @Test
    public void dummyBadInstance() throws Exception {
        SyncHelper h = new SyncHelper();

        ModelDesc desc = new ModelDesc("dummy", null, "test");
        h.loadModel(desc);

        assertNotNull(h.model.value);

        h.createInstance("nope", null);
        assertEquals("dummy: unknown instance type: nope", h.instance.error);

        h.createInstance("general", Map.of("cutoff", 40));
        assertEquals("Cutoff 40 greater than model size 22", h.instance.error);
    }

    @Test
    public void dummyGeneralOps() throws Exception {
        SyncHelper h = new SyncHelper();

        ModelDesc desc = new ModelDesc("dummy", null, "test");
        h.loadModel(desc);
        assertNotNull(h.model.value);
        h.createInstance("general", null);
        assertNotNull(h.instance.value);

        h.runInstantOp("nope", null);
        assertEquals("dummy: unknown op: nope", h.instantOp.error);

        h.runInstantOp("run", Map.of("foo", "bar"));
        assertEquals("[json.exception.out_of_range.403] key 'input' not found", h.instantOp.error);

        h.runInstantOp("run", Map.of("input", new String[]{"a", "b"}));
        assertNotNull(h.instantOp.value);
        Map val = (Map)h.instantOp.value;
        assertEquals("a one b two", val.get("result"));

        h.runInstantOp("run", Map.of("input", new String[]{"a", "b"}, "throw_on", 2));
        assertNotNull(h.instantOp.value);
        assertEquals("Throw on token 2", h.instantOp.error);
    }
}
