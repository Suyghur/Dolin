//
// Created by #Suyghur, on 4/13/21.
//

#include <jni.h>
#include "common.h"

std::string Common::TestFunc() {
    return "test func";
}

//static jstring TestLib(JNIEnv *env, jobject thiz) {
//    return env->NewStringUTF(Common::TestFunc().c_str());
//}
//
//static JNINativeMethod gMethods[] = {
//        {"testLib", "()Ljava/lang/String;", (void *) TestLib}
//
//};
//
//extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
//    JNIEnv *env = nullptr;
//    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
//        return JNI_ERR;
//    }
//    jclass clz = env->FindClass("com/suyghur/dolin/common/Common");
//    if (env->RegisterNatives(clz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
//        return JNI_ERR;
//    }
//    return JNI_VERSION_1_6;
//}
