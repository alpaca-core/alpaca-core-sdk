// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

import org.junit.Test;
import static org.junit.Assert.*;

import java.util.Map;

public class TestDict {
    static {
        System.loadLibrary("test-ac-java-dict");
    }

    private native Map getTestMapFromDict();
    private native void runCppTestWithMap(Map dict);

    @Test
    public void testCppDict() {
        Map map = getTestMapFromDict();
        assertNotNull(map);
        assertTrue(map.isEmpty());
    }
}
