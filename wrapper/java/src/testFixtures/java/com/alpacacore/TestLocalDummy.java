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
    }
}
