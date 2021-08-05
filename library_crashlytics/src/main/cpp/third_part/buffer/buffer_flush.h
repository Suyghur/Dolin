//
// Created by #Suyghur, on 2021/4/7.
//

#ifndef DOLIN_COMMON_BUFFER_FLUSH_H
#define DOLIN_COMMON_BUFFER_FLUSH_H


#include <cstdio>

class BufferFlush {

public:
    BufferFlush(FILE *log_file, size_t size = 128);

    ~BufferFlush();

    void Write(void *data, size_t len);

    void Reset();

    size_t GetLength();

    void *GetPtr();

    FILE *GetLogFile();

    void ReleaseThiz(void *buffer);

private:
    FILE *log_file_ptr = nullptr;
    void *release_ptr = nullptr;
    char *data_ptr = nullptr;
    char *write_ptr = nullptr;
    size_t capacity;

    size_t EmptySize();
};


#endif //DOLIN_COMMON_BUFFER_FLUSH_H
