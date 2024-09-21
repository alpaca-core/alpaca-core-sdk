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

    private static native Object getObjectFromEmptyDict();
    private static native Object getObjectFromPojoDict();
    private static native Object getObjectFromDictWithBinary();

    private static native void runCppTestWithNullObject(Object obj);
    private static native void runCppTestWithPojoObject(Object obj);
    private static native void runCppTestWithObjectWithBinary(Object obj);

    @Test
    public void testEmptyDict() {
        Object obj = getObjectFromEmptyDict();
        assertNull(obj);
    }

    @Test
    public void testPojoDict() {
        Map map = (Map)getObjectFromPojoDict();
        assertNotNull(map);

        /*
            "bool": true,
            "int": 42,
            "inner": {
                "str": "hello",
                "float": 3.14,
                "neg": -100,
                "big": 3000000000
            },
            "inner2": {
                "str": "world",
                "ilist": [1, "two", null, false],
                "null": null
            },
            "empty_list": [],
            "empty_dict": {}
        */
        assertEquals(6, map.size());
        assertEquals(true, map.get("bool"));
        assertEquals(42, map.get("int"));

        Map inner = (Map)map.get("inner");
        assertNotNull(inner);
        assertEquals(4, inner.size());
        assertEquals("hello", inner.get("str"));
        assertEquals(3.14, inner.get("float"));
        assertEquals(-100, inner.get("neg"));
        assertEquals(3000000000L, inner.get("big"));

        Map inner2 = (Map)map.get("inner2");
        assertNotNull(inner2);
        assertEquals(3, inner2.size());
        assertEquals("world", inner2.get("str"));

        Object[] ilist = (Object[])inner2.get("ilist");
        assertNotNull(ilist);
        assertEquals(4, ilist.length);
        assertEquals(1, ilist[0]);
        assertEquals("two", ilist[1]);
        assertNull(ilist[2]);
        assertEquals(false, ilist[3]);

        assertNull(inner2.get("null"));

        Object[] empty_list = (Object[])map.get("empty_list");
        assertNotNull(empty_list);
        assertEquals(0, empty_list.length);

        Map empty_dict = (Map)map.get("empty_dict");
        assertNotNull(empty_dict);
        assertEquals(0, empty_dict.size());
    }
}
