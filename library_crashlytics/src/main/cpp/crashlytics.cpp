//
// Created by #Suyghur, on 2021/07/30.
//

#include <jni.h>

//static void InitNative(JNIEnv *env,)

static JNINativeMethod gMethods[] = {
//        {"initNative",              "(Ljava/lang/String;Ljava/lang/String;IIZ)J", (void *) InitNative},
//        {"writeNative",             "(JLjava/lang/String;)V",                     (void *) WriteNative},
//        {"asyncFlushNative",        "(J)V",                                       (void *) AsyncFlushNative},
//        {"releaseNative",           "(J)V",                                       (void *) ReleaseNative},
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clz = env->FindClass("com/dolin/crashlytics/Crashlytics");
    if (env->RegisterNatives(clz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}