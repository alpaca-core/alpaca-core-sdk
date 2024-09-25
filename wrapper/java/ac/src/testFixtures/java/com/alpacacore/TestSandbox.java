// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
package com.alpacacore;

import org.junit.Test;
import static org.junit.Assert.*;

public class TestSandbox {
    @Test
    public void testAdd() {
        for (int i=0; i<20; ++i) {
            Sandbox a = new Sandbox(i);
            assertEquals(i+3, a.add(1, 2));
            System.gc();
        }
    }

    @Test
    public void testSeti() {
        Sandbox a = new Sandbox(0);
        a.seti(42);
        assertEquals(45, a.add(1, 2));
    }

    @Test
    public void testClone() {
        Sandbox a = new Sandbox(5);
        Sandbox b = a.clone();
        a.seti(42);
        assertEquals(45, a.add(1, 2));
        assertEquals(8, b.add(1, 2));
    }
}
