//
// Created by #Suyghur, on 2021/08/09.
//

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "hawkeye_mmap_buffer.h"
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
    this->buffer_ptr = static_cast<char *>(mmap(nullptr, _capacity, PROT_WRITE | PROT_READ, MAP_SHARED, buffer_file_fd, 0));
    if (buffer_ptr == MAP_FAILED) {
        return;
    }
    memset(buffer_ptr, '\0', _capacity);
    msync(buffer_ptr, _capacity, MS_SYNC);
}

MmapBuffer::~MmapBuffer() = default;

void MmapBuffer::Write(const char *content) {
    Write(content, true);
}

void MmapBuffer::Write(const char *content, bool appendable) {
    if (content == nullptr) {
        return;
    }
    if (appendable) {
        // 追加模式
        size_t now_len = strlen(buffer_ptr);
        size_t new_capacity = now_len + strlen(content);
        char *data_tmp = new char[new_capacity]{0};
        memcpy(data_tmp, buffer_ptr, now_len);
        strcpy(data_tmp + now_len, content);
        memcpy(buffer_ptr, data_tmp, new_capacity);
        msync(buffer_ptr, new_capacity, MS_SYNC);
        free(data_tmp);
    } else {
        // 覆盖模式
        size_t len = strlen(content);
        memcpy(buffer_ptr, content, len);
        msync(buffer_ptr, len, MS_SYNC);
    }
}

void MmapBuffer::Flush(const char *path) {
    this->file_ptr = fopen(path, "ab+");
    if (file_ptr != nullptr && buffer_ptr != nullptr) {
        ssize_t written = fwrite(buffer_ptr, strlen(buffer_ptr), 1, file_ptr);
        if (written == 1) {
            fflush(file_ptr);
        }
    }
    fclose(file_ptr);
    this->file_ptr = nullptr;
}

void MmapBuffer::Close() {
    if (buffer_ptr != nullptr) {
        munmap(buffer_ptr, strlen(buffer_ptr));
    }
    if (buffer_file_fd >= 0) {
        close(buffer_file_fd);
    }
}

char *MmapBuffer::GetBufferPtr() {
    return this->buffer_ptr;
}