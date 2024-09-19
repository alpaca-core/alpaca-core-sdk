package com.alpacacore.api;

import org.junit.Test;
import static org.junit.Assert.*;

public class TestAdder {
    @Test
    public void testAdd() {
        Adder a = new Adder(8);
        assertEquals(11, a.add(1, 2));
    }
}
