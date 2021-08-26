//
// Created by #Suyghur, on 2021/4/7.
//
#include <jni.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sstream>
#include "zap_buffer.h"
#include "zap_file_flush.h"
#include "zap_buffer_header.h"
#include "zap_log.h"
#include <map>

#ifdef __cplusplus
extern "C" {
#endif

static FileFlush *pFileFlush = nullptr;

static void WriteDirty2File(int buffer_fd, FileFlush *fileFlush) {
    struct stat file_stat{};
    if (fstat(buffer_fd, &file_stat) >= 0) {
        auto buffer_size = static_cast<size_t>(file_stat.st_size);
        //buffer size必须大于文件头长度，否则下标溢出
        if (buffer_size > dolin_zap::BufferHeader::CalculateHeaderLen(0)) {
            char *buffer_ptr_tmp = (char *) mmap(nullptr, buffer_size, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_fd, 0);
            if (buffer_ptr_tmp != MAP_FAILED) {
                auto *tmp = new Buffer(buffer_ptr_tmp, buffer_size);
                size_t data_size = tmp->GetLength();
                if (data_size > 0) {
                    tmp->CallFileFlush(fileFlush, tmp);
                } else {
                    delete tmp;
                }
            }
        }
    }
}

static char *OpenMMap(int buffer_fd, size_t buffer_size, FileFlush *fileFlush) {
    char *map_ptr = nullptr;
    if (buffer_fd != -1) {
        //写脏数据
        WriteDirty2File(buffer_fd, fileFlush);
        //根据buffer size 调整 buffer 大小
        ftruncate(buffer_fd, static_cast<int >(buffer_size));
        lseek(buffer_fd, 0, SEEK_SET);
        map_ptr = (char *) mmap(nullptr, buffer_size, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_fd, 0);
        if (map_ptr == MAP_FAILED) {
            map_ptr = nullptr;
        }
    }
    return map_ptr;
}

static jlong InitNative(JNIEnv *env, jobject thiz, jstring buffer_path, jstring log_path, jint capacity, jint limit_size, jboolean compress) {

    const char *_buffer_path = env->GetStringUTFChars(buffer_path, JNI_FALSE);
    const char *_log_path = env->GetStringUTFChars(log_path, JNI_FALSE);
    auto buffer_size = static_cast<size_t>(capacity);
    int buffer_fd = open(_buffer_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    //buffer ["路径的长度","路径","内容"], 参考mmkv格式
    auto *file_flush = new FileFlush();
    //加上头信息占用的大小
    buffer_size = buffer_size + BufferHeader::CalculateHeaderLen(strlen(_log_path));
    char *buffer_ptr = OpenMMap(buffer_fd, buffer_size, file_flush);

    bool map_buffer = true;
    //如果打开 mmap 失败，则降级使用内存缓存
    if (buffer_ptr == nullptr) {
        buffer_ptr = new char[buffer_size];
        map_buffer = false;
    }

    auto *buffer = new Buffer(buffer_ptr, buffer_size);
    buffer->CallFileFlush(file_flush);
    //将 buffer 中的数据清空，并写入日志文件信息
    buffer->InitData((char *) _log_path, strlen(_log_path), limit_size, compress);
    buffer->map_buffer = map_buffer;

    pFileFlush = file_flush;

    env->ReleaseStringUTFChars(buffer_path, _buffer_path);
    env->ReleaseStringUTFChars(log_path, _log_path);
    return reinterpret_cast<long >(buffer);
}

static void WriteNative(JNIEnv *env, jobject thiz, jlong ptr, jstring msg) {
    const char *_msg = env->GetStringUTFChars(msg, JNI_FALSE);
    jsize msg_len = env->GetStringUTFLength(msg);
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    //缓存写满时异步刷新
    if (msg_len >= buffer->EmptySize()) {
        if (pFileFlush != nullptr) {
            buffer->CallFileFlush(pFileFlush);
        }
    }
    buffer->Append(_msg, (size_t) msg_len);
    env->ReleaseStringUTFChars(msg, _msg);
}

static void AsyncFlushNative(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    if (pFileFlush != nullptr) {
        buffer->CallFileFlush(pFileFlush);
    }
}


static void ExpLogFileNative(JNIEnv *env, jobject thiz, jlong ptr, jstring path, jint limit_size) {
    const char *log_path = env->GetStringUTFChars(path, JNI_FALSE);
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    buffer->ExpLogPath(const_cast<charf *>(log_path), limit_size);
    env->ReleaseStringUTFChars(path, log_path);
}

static void ReleaseNative(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    buffer->CallFileFlush(pFileFlush, buffer);
    if (pFileFlush != nullptr) {
        delete pFileFlush;
    }
    pFileFlush = nullptr;
}

static jboolean IsLogFileOverSizeNative(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    return buffer->IsCurrentLogFileOversize() ? JNI_TRUE : JNI_FALSE;
}

static JNINativeMethod gMethods[] = {
        {"initNative",              "(Ljava/lang/String;Ljava/lang/String;IIZ)J", (void *) InitNative},
        {"writeNative",             "(JLjava/lang/String;)V",                     (void *) WriteNative},
        {"asyncFlushNative",        "(J)V",                                       (void *) AsyncFlushNative},
        {"expLogFileNative",        "(JLjava/lang/String;I)V",                    (void *) ExpLogFileNative},
        {"releaseNative",           "(J)V",                                       (void *) ReleaseNative},
        {"isLogFileOverSizeNative", "(J)Z",                                       (void *) IsLogFileOverSizeNative}};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clz = env->FindClass("com/dolin/zap/impl/ZapRecord");
    if (env->RegisterNatives(clz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif