//
// Created by #Suyghur, on 2021/08/30.
//

#include <unistd.h>
#include <fcntl.h>
#include "../hawkeye_log.h"
#include "file_guard.h"

FileGuard::FileGuard(const char *filename, size_t capacity) {
    if (filename == nullptr) {
        return;
    }
    this->fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        LOGE("open file %s failed", filename);
        return;
    }
    size_t _capacity = 0;
    if (capacity <= CAPACITY) {
        _capacity = CAPACITY;
    } else {
        _capacity = capacity;
    }
    ftruncate(fd, _capacity);
    lseek(fd, 0, SEEK_SET);
}

FileGuard::~FileGuard() {
    if (fd >= 0) {
        close(fd);
    }
}

bool FileGuard::IsOk() const {
    return fd >= 0;
}

int FileGuard::GetFd() const {
    return fd;
}

