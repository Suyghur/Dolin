//
// Created by #Suyghur, on 2021/07/30.
//

#include <jni.h>
#include <zlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "guard/mmap_guard.h"
#include "hawkeye_log.h"
#include "hawkeye_core.h"
#include "hawkeye_daemon.h"

#ifdef __cplusplus
extern "C" {
#endif

JavaVM *gJavaVm = nullptr;

typedef struct {
    jclass clz;
    jmethodID mid;
    JNIEnv *env;
} callback_context;


/// Called on daemon start from its background thread.
static void OnDaemonStart(void *argvoid) {
    auto *const ctx = static_cast<callback_context *const>(argvoid);
    gJavaVm->AttachCurrentThread(&ctx->env, nullptr);
}

/// Called when a crash report is generated. From background thread.
static void OnCrash(const char *log_file, void *argvoid) {
    auto *const ctx = static_cast<callback_context *const>(argvoid);
    jstring _log_file = ctx->env->NewStringUTF(log_file);
    ctx->env->CallStaticVoidMethod(ctx->clz, ctx->mid, _log_file);
    ctx->env->DeleteLocalRef(_log_file);
}

/// Called on daemon stop from its background thread.
static void OnDaemonStop(void *argvoid) {
    gJavaVm->DetachCurrentThread();
}

static jboolean InitCxxCrashMonitor(JNIEnv *env, jobject clz, jstring socket_name) {
    const char *_socket_name = socket_name ? env->GetStringUTFChars(socket_name, JNI_FALSE) : nullptr;
    bool success = HawkeyeCore::InitCore(_socket_name);
    char date[32];

    if (_socket_name) {
        env->ReleaseStringUTFChars(socket_name, _socket_name);
    }
    return success;
}

static jboolean ZygoteCxxCrashDaemon(JNIEnv *env, jclass clz, jstring socket_name, jstring log_path) {
    const char *_socket_name = socket_name ? env->GetStringUTFChars(socket_name, JNI_FALSE) : nullptr;
    const char *_log_path = log_path ? env->GetStringUTFChars(log_path, JNI_FALSE) : nullptr;

    auto *const ctx = static_cast<callback_context *const>(calloc(1, sizeof(callback_context)));
    ctx->clz = static_cast<jclass>(env->NewGlobalRef(clz));
    ctx->mid = env->GetStaticMethodID(ctx->clz, "onCxxCrashCallback", "(Ljava/lang/String;)V");

    const bool success = HawkeyeDaemon::StartDaemon(_socket_name, _log_path, &OnDaemonStart, &OnCrash, &OnDaemonStop, ctx);

    if (_socket_name) {
        env->ReleaseStringUTFChars(socket_name, _socket_name);
    }
    if (_log_path) {
        env->ReleaseStringUTFChars(log_path, _log_path);
    }
    return success;
}

static jboolean ReleaseCxxCrashDaemon(JNIEnv *env, jobject clz) {
    if (HawkeyeDaemon::StopDaemon()) {
        return JNI_TRUE;
    }
    return JNI_FALSE;
}

static void TestCxxCrash(JNIEnv *env, jobject clz) {
    volatile int *ptr = nullptr;
    *ptr = 1;
}

static jlong InitTempBuffer(JNIEnv *env, jobject clz, jstring buffer_path, jint capacity) {
    const char *_buffer_path = env->GetStringUTFChars(buffer_path, JNI_FALSE);
    auto buffer_size = static_cast<size_t>(capacity);
    auto buffer_ptr = new MmapGuard(_buffer_path, buffer_size);
    env->ReleaseStringUTFChars(buffer_path, _buffer_path);
    return reinterpret_cast<long >(buffer_ptr);
}

static void Record2Buffer(JNIEnv *env, jobject clz, jlong buffer_ptr, jstring content) {
    auto *_buffer_ptr = reinterpret_cast<MmapGuard *>(buffer_ptr);
    const char *_content = env->GetStringUTFChars(content, JNI_FALSE);
    _buffer_ptr->Write(_content);
    env->ReleaseStringUTFChars(content, _content);
}

static void FlushLog2File(JNIEnv *env, jobject clz, jlong buffer_ptr, jstring path) {
    auto *ptr = reinterpret_cast<MmapGuard *>(buffer_ptr);
    const char *_path = env->GetStringUTFChars(path, JNI_FALSE);
    ptr->Flush(_path);
}

static void ReleaseBuffer(JNIEnv *env, jobject clz, jlong buffer_ptr) {
    auto *_buffer_ptr = reinterpret_cast<MmapGuard *>(buffer_ptr);
    delete _buffer_ptr;
}

static JNINativeMethod gMethods[] = {
        {"initCxxCrashMonitor",   "(Ljava/lang/String;)Z",                   (void *) InitCxxCrashMonitor},
        {"zygoteCxxCrashDaemon",  "(Ljava/lang/String;Ljava/lang/String;)Z", (void *) ZygoteCxxCrashDaemon},
        {"releaseCxxCrashDaemon", "()Z",                                     (void *) ReleaseCxxCrashDaemon},
        {"testCxxCrash",          "()V",                                     (void *) TestCxxCrash},
        {"initTempBuffer",        "(Ljava/lang/String;I)J",                  (void *) InitTempBuffer},
        {"record2Buffer",         "(JLjava/lang/String;)V",                  (void *) Record2Buffer},
        {"flushLog2File",         "(JLjava/lang/String;)V",                  (void *) FlushLog2File},
        {"releaseBuffer",         "(J)V",                                    (void *) ReleaseBuffer},
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
    gJavaVm = vm;
    return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif
