//
// Created by #Suyghur, on 2021/07/30.
//

#include <jni.h>
#include <zlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "hawkeye_log.h"
#include "hawkeye_mmap_buffer.h"
#include "hawkeye_core.h"
#include "hawkeye_daemon.h"

#ifdef __cplusplus
extern "C" {
#endif


JavaVM *javaVm = nullptr;

typedef struct {
    jclass jClz;
    jmethodID jCallbackId;
    JNIEnv *daemon_thread_env;
} jCallback_arg_t;

/// Called on daemon start from its background thread.
static void DaemonStart(void *argvoid) {

    auto *const arg = static_cast<jCallback_arg_t *const>(argvoid);
    javaVm->AttachCurrentThread(&arg->daemon_thread_env, nullptr);
}

/// Called when a crash report is generated. From background thread.
static void OnCrash(const char *log_file, void *argvoid) {
    auto *const arg = static_cast<jCallback_arg_t *const> (argvoid);
    const jstring _log_file = arg->daemon_thread_env->NewStringUTF(log_file);
    arg->daemon_thread_env->CallStaticVoidMethod(arg->jClz, arg->jCallbackId, _log_file);
    arg->daemon_thread_env->DeleteLocalRef(_log_file);
}

/// Called on daemon stop from its background thread.
static void DaemonStop(void *argvoid) {
//    auto *const arg = static_cast<jCallback_arg_t *const> (argvoid);
    javaVm->DetachCurrentThread();
}

static jboolean InitCxxCrashMonitor(JNIEnv *env, jobject clz, jstring socket_name) {
    const char *_socket_name = socket_name ? env->GetStringUTFChars(socket_name, JNI_FALSE) : nullptr;
    bool success = HawkeyeCore::InitCore(_socket_name);

    if (_socket_name) {
        env->ReleaseStringUTFChars(socket_name, _socket_name);
    }
    return success;
}

static jboolean InitCxxCrashDaemon(JNIEnv *env, jobject clz, jstring socket_name, jstring log_path) {
    const char *_socket_name = socket_name ? env->GetStringUTFChars(socket_name, JNI_FALSE) : nullptr;
    const char *_log_path = log_path ? env->GetStringUTFChars(log_path, JNI_FALSE) : nullptr;
    auto *const callback_arg = static_cast<jCallback_arg_t *const>(calloc(1, sizeof(jCallback_arg_t)));
    callback_arg->jClz = env->FindClass("com/dolin/hawkeye/handler/BoostCrashHandler");
    callback_arg->jCallbackId = env->GetStaticMethodID(callback_arg->jClz, "onCxxCrashCallback", "(Ljava/lang/String;)V");
    const bool success = HawkeyeDaemon::StartDaemon(_socket_name, _log_path, &DaemonStart, &OnCrash, &DaemonStop, callback_arg);

    if (_socket_name) {
        env->ReleaseStringUTFChars(socket_name, _socket_name);
    }
    if (_log_path) {
        env->ReleaseStringUTFChars(log_path, _log_path);
    }
    return success;
}

static void TestCxxCrash(JNIEnv *env, jobject clz) {
    volatile int *ptr = nullptr;
    *ptr = 1;
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
        {"initCxxCrashMonitor", "(Ljava/lang/String;)Z",                   (void *) InitCxxCrashMonitor},
        {"initCxxCrashDaemon",  "(Ljava/lang/String;Ljava/lang/String;)Z", (void *) InitCxxCrashDaemon},
        {"testCxxCrash",        "()V",                                     (void *) TestCxxCrash},
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
    javaVm = vm;
    return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif
