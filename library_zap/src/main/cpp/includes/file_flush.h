//
// Created by #Suyghur, on 4/7/21.
//

#ifndef DOLIN_ZAP_FILE_FLUSH_H
#define DOLIN_ZAP_FILE_FLUSH_H


#include "buffer_flush.h"
#include <vector>
#include <thread>

class FileFlush {
public:
    FileFlush();

    ~FileFlush();

    bool asyncFlush(BufferFlush *buffer);

    void stopFlush();

private:
    void asyncLogThread();

    static ssize_t flush(BufferFlush *buffer);

    bool exit = false;
    std::vector<BufferFlush *> async_buffer;
    std::thread async_thread;
    std::condition_variable async_condition;
    std::mutex async_mtx;
};


#endif //DOLIN_ZAP_FILE_FLUSH_H
