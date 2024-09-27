// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

import org.junit.Test;
import static org.junit.Assert.*;

import java.util.Map;

public class TestLocalDummy {
    @Test
    public void releaseNull() {
        AlpacaCore.releaseModel(null);
        AlpacaCore.releaseInstance(null);
    }

    class ProgressBox {
        String tag;
        float progress;
    }

    @Test
    public void localDummy() {
        ModelDesc desc = new ModelDesc("nope", null, null);
        try {
            Model model = AlpacaCore.createModel(desc, null, null);
            fail("Should have thrown an exception");
        }
        catch (Error e) {
            assertEquals("Unknown inference type: nope", e.getMessage());
        }

        desc.inferenceType = "dummy";
        desc.name = "synthetic dummy";
        ProgressBox pdata = new ProgressBox();
        Model model = AlpacaCore.createModel(desc, null, new ProgressCallback() {
            @Override
            public boolean onProgress(String tag, float progress) {
                pdata.tag = tag;
                pdata.progress = progress;
                return true;
            }
        });
        assertNotNull(model);
        assertEquals("synthetic", pdata.tag);
        assertEquals(0.5, pdata.progress, 0.0001);

        try {
            Instance instance = model.createInstance("nope", null);
            fail("Should have thrown an exception");
        }
        catch (Error e) {
            assertEquals("dummy: unknown instance type: nope", e.getMessage());
        }

        try {
            Instance instance = model.createInstance("general", Map.of("cutoff", 100));
            fail("Should have thrown an exception");
        }
        catch (Error e) {
            assertEquals("Cutoff 100 greater than model size 22", e.getMessage());
        }

        Instance instance = model.createInstance("general", null);
        assertNotNull(instance);

        try {
            instance.runOp("nope", null, null);
            fail("Should have thrown an exception");
        }
        catch (Error e) {
            assertEquals("dummy: unknown op: nope", e.getMessage());
        }

        Map result = (Map)instance.runOp("run", Map.of("input", new String[]{"a", "b"}), null);
        assertNotNull(result);
        String opResult = (String)result.get("result");
        assertEquals("a one b two", opResult);

        try {
            instance.runOp("run", Map.of("input", new String[]{"a", "b", "c"}, "throw_on", 3), null);
            fail("Should have thrown an exception");
        }
        catch (Error e) {
            assertEquals("Throw on token 3", e.getMessage());
        }

        Instance cutoffInstance = model.createInstance("general", Map.of("cutoff", 2));
        assertNotNull(cutoffInstance);

        result = (Map)cutoffInstance.runOp("run", Map.of("input", new String[]{"a", "b", "c"}), null);
        assertNotNull(result);
        opResult = (String)result.get("result");
        assertEquals("a one b two c one", opResult);

        AlpacaCore.releaseInstance(cutoffInstance);
        AlpacaCore.releaseInstance(instance);
        AlpacaCore.releaseModel(model);
    }
}
