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

    private static native Map getMapFromEmptyDict();
    private static native Map getMapFromPojoDict();
    private static native Map getMapFromDictWithBinary();

    private static native void runCppTestWithNullMap(Map map);
    private static native void runCppTestWithPojoMap(Map map);
    private static native void runCppTestWithMapWithBinary(Map map);

    @Test
    public void testEmptyDict() {
        Map map = getMapFromEmptyDict();
        assertNull(map);
    }

    @Test
    public void testPojoDict() {
        Map map = getMapFromPojoDict();
        assertNotNull(map);
        assertEquals(6, map.size());
    }
}
