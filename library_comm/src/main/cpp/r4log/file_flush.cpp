//
// Created by #Suyghur, on 2021/4/7.
//

#include "file_flush.h"

#include <android/log.h>

#define TAG "dolin_jni"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__) // 定义LOGD类型
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__) // 定义LOGD类型

FileFlush::FileFlush() {
    async_thread = std::thread(&FileFlush::AsyncLogThread, this);
}

FileFlush::~FileFlush() {
    StopFlush();
}

bool FileFlush::AsyncFlush(BufferFlush *buffer) {
    std::unique_lock<std::mutex> lck_async_flush(async_mtx);
    if (exit) {
        delete buffer;
        return false;
    }
    async_buffer.push_back(buffer);
    async_condition.notify_all();
    return true;
}

void FileFlush::StopFlush() {
    exit = true;
    async_condition.notify_all();
    async_thread.join();
    if (rename_path != nullptr) {
        delete[] rename_path;
    }
}

void FileFlush::AsyncLogThread() {
    while (true) {
        std::unique_lock<std::mutex> lck_async_log_thread(async_mtx);
        while (!async_buffer.empty()) {
            BufferFlush *data = async_buffer.back();
            async_buffer.pop_back();
            Flush(data, rename_path);
        }
        if (exit) {
            return;
        }
        async_condition.wait(lck_async_log_thread);
    }
}

/**
 * 写文件
 */
ssize_t FileFlush::Flush(BufferFlush *buffer) {
    return Flush(buffer, nullptr);
}

ssize_t FileFlush::Flush(BufferFlush *buffer, const char *rename_path) {
    ssize_t written = 0;
    FILE *log_file = buffer->GetLogFile();
    if (log_file != nullptr && buffer->GetLength() > 0) {
        written = fwrite(buffer->GetPtr(), buffer->GetLength(), 1, log_file);
        fflush(log_file);
        if (rename_path != nullptr) {
//            rename(buffer->GetLogPath())
//            rename_path(buffer->GetPtr());

            LOGD("tmp path : %s", buffer->GetLogPath());
            LOGD("rename path : %s", rename_path);
            rename(buffer->GetLogPath(), rename_path);
        }
    }
    delete buffer;
    return written;
}
