//
// Created by #Suyghur, on 2021/08/30.
//

#include <cstring>
#include <sys/mman.h>
#include <cstdlib>
#include "mmap_guard.h"

MmapGuard::MmapGuard(const char *filename, size_t capacity) {
    if (filename == nullptr) {
        return;
    }
    size_t _capacity = 0;
    if (capacity <= CAPACITY) {
        _capacity = CAPACITY;
    } else {
        _capacity = capacity;
    }
    this->file_guard = new FileGuard(filename, capacity);
    if (!file_guard->IsOk()) {
        return;
    }
    this->buffer_ptr = static_cast<char *>(mmap(nullptr, _capacity, PROT_WRITE | PROT_READ, MAP_SHARED, file_guard->GetFd(), 0));
    if (buffer_ptr == MAP_FAILED) {
        return;
    }
    memset(buffer_ptr, '\0', _capacity);
    msync(buffer_ptr, _capacity, MS_SYNC);
}

MmapGuard::~MmapGuard() {
    if (buffer_ptr != nullptr) {
        munmap(buffer_ptr, strlen(buffer_ptr));
        buffer_ptr = nullptr;
    }
    if (file_ptr != nullptr) {
        fclose(file_ptr);
        file_ptr = nullptr;
    }
    if (file_guard != nullptr) {
        delete file_guard;
        file_ptr = nullptr;
    }

}

void MmapGuard::Write(const char *content) {
    this->Write(content, strlen(content));
}

void MmapGuard::Write(const char *content, size_t content_len) {
    this->Write(content, content_len, true);
}

void MmapGuard::Write(const char *content, size_t content_len, bool appendable) {
    if (content == nullptr) {
        return;
    }
    if (buffer_ptr == nullptr) {
        return;
    }
    if (appendable) {
        // 追加模式
        size_t now_len = strlen(buffer_ptr);
        size_t new_capacity = now_len + content_len;
        char *data_tmp = new char[new_capacity]{0};
        memcpy(data_tmp, buffer_ptr, now_len);
        strcpy(data_tmp + now_len, content);
        memcpy(buffer_ptr, data_tmp, new_capacity);
        msync(buffer_ptr, new_capacity, MS_SYNC);
        free(data_tmp);
    } else {
        // 覆盖模式
        memcpy(buffer_ptr, content, content_len);
        msync(buffer_ptr, content_len, MS_SYNC);
    }
}

void MmapGuard::Flush(const char *filename) {
    this->file_ptr = fopen(filename, "ab+");
    if (file_ptr != nullptr && buffer_ptr != nullptr) {
        ssize_t written = fwrite(buffer_ptr, strlen(buffer_ptr), 1, file_ptr);
        if (written == 1) {
            fflush(file_ptr);
        }
    }
    fclose(file_ptr);
    this->file_ptr = nullptr;
}

bool MmapGuard::IsOk() const {
    return buffer_ptr != nullptr;
}

char *MmapGuard::GetBufferPtr() const {
    return buffer_ptr;
}


