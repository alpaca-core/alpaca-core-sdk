package com.alpacacore;

import androidx.test.ext.junit.runners.AndroidJUnit4;

import org.junit.runner.RunWith;

import org.junit.Test;
import static org.junit.Assert.*;

import java.util.concurrent.CountDownLatch;

@RunWith(AndroidJUnit4.class)
public class InTestSandbox {
    @Test
    public void testSandbox() throws Exception {
        ModelDesc desc = new ModelDesc("nope", null, null);
        assertEquals(desc.inferenceType, "nope");
        LocalProvider.sandbox(desc);
        CountDownLatch latch = new CountDownLatch(1);
        LocalProvider.loadModel(desc, null, new LocalProvider.LoadModelCallback() {
            @Override
            public void onComplete(Model result) {
                latch.countDown();
            }

            @Override
            public void onError(String error) {
                latch.countDown();
            }

            @Override
            public void onProgress(String tag, float progress) {}
        });
        latch.await();
    }
}
