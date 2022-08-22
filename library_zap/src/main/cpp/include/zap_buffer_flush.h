//
// Created by #Suyghur, on 2021/4/7.
//

#ifndef DOLIN_ZAP_BUFFER_FLUSH_H
#define DOLIN_ZAP_BUFFER_FLUSH_H

#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

class BufferFlush {

public:
    BufferFlush(FILE *log_file, char *path, size_t size = 128);

    ~BufferFlush();

    void Write(void *data, size_t len);

    void Reset();

    size_t GetLength();

    void *GetPtr();

    FILE *GetLogFile();

    char *GetLogPath();

    void ReleaseThiz(void *buffer);

private:
    FILE *log_file_ptr = nullptr;
    void *release_ptr = nullptr;
    char *data_ptr = nullptr;
    char *write_ptr = nullptr;
    char *path = nullptr;
    size_t capacity;

    size_t EmptySize();
};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_ZAP_BUFFER_FLUSH_H
