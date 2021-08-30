//
// Created by #Suyghur, on 2021/08/30.
//

#ifndef DOLIN_MMAP_GUARD_H
#define DOLIN_MMAP_GUARD_H

#include <stdio.h>
#include "file_guard.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define CAPACITY 1024*400

class MmapGuard {

public:
    MmapGuard(const char *filename, size_t capacity);

    ~MmapGuard();

    void Write(const char *content);

    void Write(const char *content, size_t content_len);

    void Write(const char *content, size_t content_len, bool appendable);

    void Flush(const char *filename);

    bool IsOk() const;

    char *GetBufferPtr() const;

private:
    char *buffer_ptr = nullptr;

    FILE *file_ptr = nullptr;

    FileGuard *file_guard = nullptr;
};

#if defined(__cplusplus)
}
#endif

#endif //DOLIN_MMAP_GUARD_H
