//
// Created by #Suyghur, on 4/7/21.
//
#include <jni.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sstream>
#include "libs/buffer.h"
#include "libs/file_flush.h"
#include "libs/buffer_header.h"

static FileFlush *p_file_flush = nullptr;

static void writeDirty2File(int buffer_fd) {
    struct stat file_stat{};
    if (fstat(buffer_fd, &file_stat) >= 0) {
        auto buffer_size = static_cast<size_t>(file_stat.st_size);
        //buffer size必须大于文件头长度，否则下标溢出
        if (buffer_size > buffer_header::BufferHeader::calculateHeaderLen(0)) {
            char *buffer_ptr_tmp = (char *) mmap(0, buffer_size, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_fd, 0);
            if (buffer_ptr_tmp != MAP_FAILED) {
                auto *tmp = new Buffer(buffer_ptr_tmp, buffer_size);
                size_t data_size = tmp->getLength();
                if (data_size > 0) {
                    tmp->callFileFlush(p_file_flush, tmp);
                } else {
                    delete tmp;
                }
            }
        }
    }
}

static char *openMMap(int buffer_fd, size_t buffer_size) {
    char *map_ptr = nullptr;
    if (buffer_fd != -1) {
        //写脏数据
        writeDirty2File(buffer_fd);
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


static jlong initNative(JNIEnv *env, jobject thiz, jstring buffer_path, jint capacity,
                        jstring log_path, jboolean compress) {
    const char *_buffer_path = env->GetStringUTFChars(buffer_path, JNI_FALSE);
    const char *_log_path = env->GetStringUTFChars(log_path, JNI_FALSE);
    auto buffer_size = static_cast<size_t>(capacity);
    int buffer_fd = open(_buffer_path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    //buffer ["路径的长度","路径","内容"], 参考mmkv格式
    if (p_file_flush == nullptr) {
        p_file_flush = new FileFlush();
    }
    //加上头信息占用的大小
    buffer_size = buffer_size + BufferHeader::calculateHeaderLen(strlen(_log_path));
    char *buffer_ptr = openMMap(buffer_fd, buffer_size);

    bool map_buffer = true;
    //如果打开 mmap 失败，则降级使用内存缓存
    if (buffer_ptr == nullptr) {
        buffer_ptr = new char[buffer_size];
        map_buffer = false;
    }

    auto *buffer = new Buffer(buffer_ptr, buffer_size);
    buffer->callFileFlush(p_file_flush);
    //将 buffer 中的数据清空，并写入日志文件信息
    buffer->initData((char *) _log_path, strlen(_log_path), compress);
    buffer->map_buffer = map_buffer;

    env->ReleaseStringUTFChars(buffer_path, _buffer_path);
    env->ReleaseStringUTFChars(log_path, _log_path);
    return reinterpret_cast<long >(buffer);
}

static void writeNative(JNIEnv *env, jobject thiz, jlong ptr, jstring msg) {
    const char *_msg = env->GetStringUTFChars(msg, JNI_FALSE);
    jsize msg_len = env->GetStringUTFLength(msg);
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    //缓存写满时异步刷新
    if (msg_len >= buffer->emptySize()) {
        if (p_file_flush != nullptr) {
            buffer->callFileFlush(p_file_flush);
        }
    }
    buffer->append(_msg, (size_t) msg_len);
    env->ReleaseStringUTFChars(msg, _msg);
}

static void asyncFlushNative(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    if (p_file_flush != nullptr) {
        buffer->callFileFlush(p_file_flush);
    }
}

static void changeLogPathNative(JNIEnv *env, jobject thiz, jlong ptr, jstring path) {
    const char *log_path = env->GetStringUTFChars(path, JNI_FALSE);
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    buffer->changeLogPath(const_cast<charf *>(log_path));
    env->ReleaseStringUTFChars(path, log_path);
}

static void releaseNative(JNIEnv *env, jobject thiz, jlong ptr) {
    auto *buffer = reinterpret_cast<Buffer *>(ptr);
    buffer->callFileFlush(p_file_flush, buffer);
    if (p_file_flush != nullptr) {
        delete p_file_flush;
    }
    p_file_flush = nullptr;
}


static JNINativeMethod gMethods[] = {
        {"initNative",          "(Ljava/lang/String;ILjava/lang/String;Z)J", (void *) initNative},
        {"writeNative",         "(JLjava/lang/String;)V",                    (void *) writeNative},
        {"asyncFlushNative",    "(J)V",                                      (void *) asyncFlushNative},
        {"changeLogPathNative", "(JLjava/lang/String;)V",                    (void *) changeLogPathNative},
        {"releaseNative",       "(J)V",                                      (void *) releaseNative},

};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clz = env->FindClass("com/suyghur/dolin/logger/record/RecordImpl");
    if (env->RegisterNatives(clz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}