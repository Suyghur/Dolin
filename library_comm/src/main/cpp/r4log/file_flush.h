//
// Created by #Suyghur, on 2021/4/7.
//

#ifndef DOLIN_FILE_FLUSH_H
#define DOLIN_FILE_FLUSH_H


#include "buffer_flush.h"
#include <vector>
#include <thread>

class FileFlush {
public:
    FileFlush();

    ~FileFlush();

    bool AsyncFlush(BufferFlush *buffer);

    void StopFlush();

    char *rename_path = nullptr;

private:
    void AsyncLogThread();

    static ssize_t Flush(BufferFlush *buffer);

    static ssize_t Flush(BufferFlush *buffer, const char *rename_path);

    bool exit = false;
    std::vector<BufferFlush *> async_buffer;
    std::thread async_thread;
    std::condition_variable async_condition;
    std::mutex async_mtx;
};


#endif //DOLIN_FILE_FLUSH_H
