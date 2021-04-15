//
// Created by #Suyghur, on 4/7/21.
//

#include "buffer.h"
#include "common_log.h"

Buffer::Buffer(char *ptr, size_t buffer_size) : buffer_ptr(ptr), buffer_size(buffer_size), buffer_header(buffer_ptr, buffer_size) {}

Buffer::~Buffer() {
    Release();
}

void Buffer::InitData(char *log_path, size_t log_path_len, bool _compress) {
    std::lock_guard<std::recursive_mutex> lck_release(log_mtx);
    memset(buffer_ptr, '\0', buffer_size);

    dolin_common::Header header{};
    header.magic = kMagicHeader;
    header.log_path_len = log_path_len;
    header.log_path = log_path;
    header.log_len = 0;
    header.compress = _compress;

    buffer_header.InitHeader(header);
    InitCompress(_compress);

    data_ptr = (char *) buffer_header.GetPtr();
    write_ptr = (char *) buffer_header.GetWritePtr();

    OpenLogFile(log_path);
}

void Buffer::SetLength(size_t len) {
    buffer_header.SetLogLen(len);
}

size_t Buffer::GetLength() {
    return write_ptr - data_ptr;
}

size_t Buffer::Append(const char *log, size_t len) {
    LOGD("JNI->%s", log);
    std::lock_guard<std::recursive_mutex> lck_append(log_mtx);
    if (GetLength() == 0) {
        InitCompress(compress);
    }

    size_t free_size = EmptySize();
    size_t write_size;
    if (compress) {
        zStream.avail_in = (uInt) len;
        zStream.next_in = (Bytef *) log;

        zStream.avail_out = (uInt) free_size;
        zStream.next_out = (Bytef *) write_ptr;

        if (Z_OK != deflate(&zStream, Z_SYNC_FLUSH)) {
            return 0;
        }

        write_size = free_size - zStream.avail_out;
    } else {
        write_size = len <= free_size ? len : free_size;
        memcpy(write_ptr, log, write_size);
    }
    write_ptr += write_size;
    SetLength(GetLength());
    return write_size;
}

void Buffer::Release() {
    std::lock_guard<std::recursive_mutex> lck_release(log_mtx);
    if (compress && Z_NULL != zStream.state) {
        deflateEnd(&zStream);
    }
    if (map_buffer) {
        munmap(buffer_ptr, buffer_size);
    } else {
        delete[]buffer_ptr;
    }
    if (log_file_ptr != nullptr) {
        fclose(log_file_ptr);
    }
}

size_t Buffer::EmptySize() {
    return buffer_size - (write_ptr - buffer_ptr);
}

char *Buffer::GetLogPath() {
    return buffer_header.GetLogPath();
}

void Buffer::SetFileFlush(FileFlush *flush) {
    file_flush_ptr = flush;
}

void Buffer::CallFileFlush() {
    CallFileFlush(file_flush_ptr);
}

void Buffer::CallFileFlush(FileFlush *flush) {
    CallFileFlush(flush, nullptr);
}

void Buffer::CallFileFlush(FileFlush *flush, Buffer *buffer) {
    if (flush == nullptr) {
        if (buffer != nullptr) {
            delete buffer;
        }
        return;
    }

    std::lock_guard<std::recursive_mutex> lck_flush(log_mtx);
    if (GetLength() > 0) {
        if (compress && Z_NULL != zStream.state) {
            deflateEnd(&zStream);
        }
        auto *buffer_flush = new BufferFlush(log_file_ptr);
        buffer_flush->Write(data_ptr, GetLength());
        buffer_flush->ReleaseThiz(buffer);
        Clear();
        flush->AsyncFlush(buffer_flush);
    } else {
        delete buffer;
    }
}

void Buffer::ChangeLogPath(char *path) {
    if (log_file_ptr != nullptr) {
        CallFileFlush();
    }
    InitData(path, strlen(path), compress);
}

void Buffer::Clear() {
    std::lock_guard<std::recursive_mutex> lck_clear(log_mtx);
    write_ptr = data_ptr;
    memset(write_ptr, '\0', EmptySize());
    SetLength(GetLength());
}

bool Buffer::InitCompress(bool _compress) {
    compress = _compress;
    if (compress) {
        zStream.zalloc = Z_NULL;
        zStream.zfree = Z_NULL;
        zStream.opaque = Z_NULL;
        return Z_OK == deflateInit2(&zStream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    }
    return false;
}

bool Buffer::OpenLogFile(const char *path) {
    if (path != nullptr) {
        FILE *file = fopen(path, "ab+");
        if (file != nullptr) {
            log_file_ptr = file;
            return true;
        }
    }
    return false;
}
