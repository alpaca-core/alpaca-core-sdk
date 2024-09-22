// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore.api;

import org.junit.Test;
import static org.junit.Assert.*;

import java.util.Map;
import java.util.HashMap;

public class TestDict {
    static {
        System.loadLibrary("test-ac-java-dict");
    }

    private static native Object getObjectFromDictByJson(String json);
    private static native Object getObjectFromDictWithBinary();

    private static native boolean runCppTestWithNullObject(Object obj);
    private static native int runCppTestWithJsonLikeObject(Object obj);
    private static native boolean runCppTestWithObjectWithBinary(Object obj);

    @Test
    public void testEmptyDict() {
        Object obj = getObjectFromDictByJson("");
        assertNull(obj);
    }

    @Test
    public void testBasicDict() {
        Map map = (Map)getObjectFromDictByJson("""
        {
            "bool": true,
            "int": 42,
            "str": "hello",
            "float": 3.14
        }
        """);
        assertNotNull(map);

        assertEquals(4, map.size());
        assertEquals(true, map.get("bool"));
        assertEquals(42, map.get("int"));
        assertEquals("hello", map.get("str"));
        assertEquals(3.14, map.get("float"));
    }

    @Test
    public void testPojoDict() {
        Map map = (Map)getObjectFromDictByJson("""
        {
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
                "ilist": [1, "two", null, false, {"key": 1, "key2": "val"}],
                "null": null
            },
            "empty_list": [],
            "empty_dict": {}
        }
        """);
        assertNotNull(map);

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
        assertEquals(5, ilist.length);
        assertEquals(1, ilist[0]);
        assertEquals("two", ilist[1]);
        assertNull(ilist[2]);
        assertEquals(false, ilist[3]);
        Map listobj = (Map)ilist[4];
        assertNotNull(listobj);
        assertEquals(2, listobj.size());
        assertEquals(1, listobj.get("key"));
        assertEquals("val", listobj.get("key2"));

        assertNull(inner2.get("null"));

        Object[] empty_list = (Object[])map.get("empty_list");
        assertNotNull(empty_list);
        assertEquals(0, empty_list.length);

        Map empty_dict = (Map)map.get("empty_dict");
        assertNotNull(empty_dict);
        assertEquals(0, empty_dict.size());
    }

    @Test
    public void testNullObject() {
        assertTrue(runCppTestWithNullObject(null));
    }

    @Test
    public void testPojoObject() {
        Map map = new HashMap() ;
        map.put("false", false);
        map.put("null", null);
        map.put("int", 101);
        map.put("long_i", 5L);
        map.put("long_u", 3000000000L);
        map.put("long_d", 5000000000L);
        map.put("long_d2", -3000000000L);
        map.put("str", "hello");

        Map obj1 = new HashMap();
        obj1.put("key", 1);
        obj1.put("pi", 3.14);
        obj1.put("empty_list", new Object[0]);
        obj1.put("full_list", new Object[]{1, "horse", false});

        map.put("obj", obj1);

        Map inAr = new HashMap();
        inAr.put("key", 1);
        inAr.put("key2", "val");
        Object[] arr = {inAr, true, 0.5, "world"};
        map.put("arr", arr);

        assertEquals(0, runCppTestWithJsonLikeObject(map));
    }
}
