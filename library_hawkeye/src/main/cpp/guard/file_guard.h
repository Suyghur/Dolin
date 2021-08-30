//
// Created by #Suyghur, on 2021/08/30.
//

#ifndef DOLIN_FILE_GUARD_H
#define DOLIN_FILE_GUARD_H

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define CAPACITY 1024*400

class FileGuard {

public:
    FileGuard(const char *filename, size_t capacity);

    ~FileGuard();

    bool IsOk() const;

    int GetFd() const;

private:

    int fd = -1;

};

#if defined(__cplusplus)
}
#endif

#endif //DOLIN_FILE_GUARD_H
