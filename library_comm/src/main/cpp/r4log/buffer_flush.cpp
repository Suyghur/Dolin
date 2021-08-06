//
// Created by #Suyghur, on 2021/4/7.
//

#include <cmath>
#include <cstring>
#include "buffer_flush.h"

BufferFlush::BufferFlush(FILE *log_file, char *path, size_t size) : capacity(size), log_file_ptr(log_file), path(path) {}

BufferFlush::~BufferFlush() {
    if (path != nullptr) {
        delete[] path;
    }

    if (write_ptr != nullptr) {
        delete[] write_ptr;
    }

    if (data_ptr != nullptr) {
        delete[] data_ptr;
    }
    if (release_ptr != nullptr) {
//        delete release_ptr;
        ::operator delete(release_ptr);
    }

}

/**
 * 填充缓存
 */
void BufferFlush::Write(void *data, size_t len) {
    if (data_ptr == nullptr) {
        capacity = (size_t) fmax(capacity, len);
        data_ptr = new char[capacity]{0};
        write_ptr = data_ptr;
    }


    size_t empty_size = EmptySize();
    if (len < empty_size) {
        memcpy(write_ptr, data, len);
        write_ptr += len;
    } else {
        size_t now_len = GetLength();
        size_t new_capacity = now_len + len;
        char *data_tmp = new char[new_capacity]{0};
        memcpy(data_tmp, data_ptr, now_len);
        memcpy(data_tmp + now_len, data, len);
        char *old_data = data_ptr;
        data_ptr = data_tmp;
        write_ptr = data_ptr + new_capacity;
        delete[]old_data;
    }
}

void BufferFlush::Reset() {
    if (data_ptr != nullptr) {
        memset(data_ptr, 0, capacity);
        write_ptr = data_ptr;
    }
}

size_t BufferFlush::GetLength() {
    if (data_ptr != nullptr && write_ptr != nullptr) {
        return write_ptr - data_ptr;
    }
    return 0;
}

void *BufferFlush::GetPtr() {
    return data_ptr;
}

FILE *BufferFlush::GetLogFile() {
    return log_file_ptr;
}

void BufferFlush::ReleaseThiz(void *buffer) {
    this->release_ptr = buffer;
}

size_t BufferFlush::EmptySize() {
    return capacity - GetLength();
}

char *BufferFlush::GetLogPath() {
    return path;
}
