//
// Created by #Suyghur, on 2021/08/09.
//

#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include "hawkeye_mmap_buffer.h"
#include <unistd.h>
#include "hawkeye_log.h"


MmapBuffer::MmapBuffer(const char *path, size_t capacity) {
    if (path == nullptr) {
        return;
    }
    size_t _capacity = 0;
    if (capacity <= 1024 * 400) {
        _capacity = 1024 * 400;
    }
    this->buffer_file_fd = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (buffer_file_fd < 0) {
        LOGE("open file %s failed", path);
        return;
    }
    ftruncate(buffer_file_fd, _capacity);
    lseek(buffer_file_fd, 0, SEEK_SET);
    this->buffer_ptr = (long *) mmap(nullptr, _capacity, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_file_fd, 0);
    if (buffer_ptr == MAP_FAILED) {
        return;
    }
    memset(buffer_ptr, '\0', _capacity);
    msync(buffer_ptr, _capacity, MS_SYNC);
}

MmapBuffer::~MmapBuffer() = default;

void MmapBuffer::WriteBuffer(const char *content) {
    int len = strlen(content);
    memcpy(buffer_ptr, content, len);
    msync(buffer_ptr, len, MS_SYNC);
}

void MmapBuffer::CloseBuffer() {
    if (buffer_ptr != nullptr) {
        munmap(buffer_ptr, content_len);
    }
    if (buffer_file_fd >= 0) {
        close(buffer_file_fd);
    }
}

