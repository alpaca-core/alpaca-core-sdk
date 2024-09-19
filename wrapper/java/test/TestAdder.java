package com.alpacacore.api;

import org.junit.Test;
import static org.junit.Assert.*;

public class TestAdder {
    @Test
    public void testAdd() {
        assertEquals(3, Adder.add(1, 2));
    }
}
