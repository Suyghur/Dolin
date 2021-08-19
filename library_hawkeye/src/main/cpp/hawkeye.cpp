//
// Created by #Suyghur, on 2021/07/30.
//

#include <jni.h>
#include <zlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "log.h"
#include "hawkeye_mmap_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

static jboolean InitCxxCrashMonitor(JNIEnv *env, jobject clz) {
    LOGD("InitCxxCrashMonitor ...");
    return JNI_OK;
}

static jlong Record2Buffer(JNIEnv *env, jobject clz, jstring buffer_path, jstring content) {
    const char *_buffer_path = env->GetStringUTFChars(buffer_path, JNI_FALSE);
    const char *_content = env->GetStringUTFChars(content, JNI_FALSE);
    auto *buffer_ptr = new MmapBuffer();
    buffer_ptr->WriteBuffer(_buffer_path, _content);
    env->ReleaseStringUTFChars(buffer_path, _buffer_path);
    env->ReleaseStringUTFChars(content, _content);
    return reinterpret_cast<long >(buffer_ptr);
}

static void ReleaseBuffer(JNIEnv *env, jobject clz, jlong buffer_ptr) {
    auto *_buffer_ptr = reinterpret_cast<MmapBuffer *>(buffer_ptr);
    _buffer_ptr->CloseBuffer();
    delete _buffer_ptr;
}

static JNINativeMethod gMethods[] = {
        {"initCxxCrashMonitor", "()Z",                                     (void *) InitCxxCrashMonitor},
        {"record2Buffer",       "(Ljava/lang/String;Ljava/lang/String;)J", (void *) Record2Buffer},
        {"releaseBuffer",       "(J)V",                                    (void *) ReleaseBuffer},
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clz = env->FindClass("com/dolin/hawkeye/handler/BoostCrashHandler");
    if (env->RegisterNatives(clz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif
