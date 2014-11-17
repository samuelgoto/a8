#include <string.h>
#include <jni.h>

jstring Java_com_example_test_FullscreenActivity_invokeNativeFunction(JNIEnv* env, jobject javaThis) {
  return (*env)->NewStringUTF(env, "Hello world!!!");
}
