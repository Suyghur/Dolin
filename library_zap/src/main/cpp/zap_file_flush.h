//
// Created by #Suyghur, on 2021/4/7.
//

#ifndef DOLIN_ZAP_FILE_FLUSH_H
#define DOLIN_ZAP_FILE_FLUSH_H

#include "zap_buffer_flush.h"
#include <vector>
#include <thread>

#ifdef __cplusplus
extern "C" {
#endif

class FileFlush {
public:
    FileFlush();

    ~FileFlush();

    bool AsyncFlush(BufferFlush *buffer);

    void StopFlush();

private:
    void AsyncLogThread();

    static ssize_t Flush(BufferFlush *buffer);

    bool exit = false;

    std::vector<BufferFlush *> async_buffer;

    std::thread async_thread;

    std::condition_variable async_condition;

    std::mutex async_mtx;
};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_ZAP_FILE_FLUSH_H
