//
// Created by #Suyghur, on 2021/08/09.
//

#ifndef DOLIN_HAWKEYE_MMAP_BUFFER_H
#define DOLIN_HAWKEYE_MMAP_BUFFER_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

class MmapBuffer {
public:

    MmapBuffer(const char *path, size_t capacity);

    ~MmapBuffer();

    void Write(const char *content);

    void Write(const char *content, bool appendable);

    void Flush(const char *path);

    void Close();

    char *GetBufferPtr();


private:
    int buffer_file_fd = -1;

    char *buffer_ptr = nullptr;

    FILE *file_ptr = nullptr;
};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_HAWKEYE_MMAP_BUFFER_H
