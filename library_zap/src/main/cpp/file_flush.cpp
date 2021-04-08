//
// Created by #Suyghur, on 4/7/21.
//

#include "includes/file_flush.h"

FileFlush::FileFlush() {
    async_thread = std::thread(&FileFlush::asyncLogThread, this);
}

FileFlush::~FileFlush() {
    stopFlush();
}

bool FileFlush::asyncFlush(BufferFlush *buffer) {
    std::unique_lock<std::mutex> lck_async_flush(async_mtx);
    if (exit) {
        delete buffer;
        return false;
    }
    async_buffer.push_back(buffer);
    async_condition.notify_all();
    return true;
}

void FileFlush::stopFlush() {
    exit = true;
    async_condition.notify_all();
    async_thread.join();
}

void FileFlush::asyncLogThread() {
    while (true) {
        std::unique_lock<std::mutex> lck_async_log_thread(async_mtx);
        while (!async_buffer.empty()) {
            BufferFlush *data = async_buffer.back();
            async_buffer.pop_back();
            flush(data);
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
ssize_t FileFlush::flush(BufferFlush *buffer) {
    ssize_t written = 0;
    FILE *log_file = buffer->getLogFile();
    if (log_file != nullptr && buffer->getLength() > 0) {
        written = fwrite(buffer->getPtr(), buffer->getLength(), 1, log_file);
        fflush(log_file);
    }
    delete buffer;
    return written;
}
