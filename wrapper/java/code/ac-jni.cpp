#include <jni.h>

extern "C" {

JNIEXPORT jint JNICALL
    Java_com_alpacacore_api_Adder_add(JNIEnv*, jclass, jint a, jint b) {
    return a + b;
}

}
