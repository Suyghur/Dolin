//
// Created by #Suyghur, on 4/7/21.
//

#include "buffer.h"

Buffer::Buffer(char *ptr, size_t buffer_size) : buffer_ptr(ptr), buffer_size(buffer_size), buffer_header(buffer_ptr, buffer_size) {}

Buffer::~Buffer() {
    release();
}

void Buffer::initData(char *log_path, size_t log_path_len, bool compress) {
    std::lock_guard<std::recursive_mutex> lck_release(log_mtx);
    memset(buffer_ptr, '\0', buffer_size);

    buffer_header::Header header{};
    header.magic = kMagicHeader;
    header.log_path_len = log_path_len;
    header.log_path = log_path;
    header.log_len = 0;
    header.compress = compress;

    buffer_header.initHeader(header);
    initCompress(compress);

    data_ptr = (char *) buffer_header.getPtr();
    write_ptr = (char *) buffer_header.getWritePtr();

    openLogFile(log_path);
}

void Buffer::setLength(size_t len) {
    buffer_header.setLogLen(len);
}

size_t Buffer::getLength() {
    return write_ptr - data_ptr;
}

size_t Buffer::append(const char *log, size_t len) {
    std::lock_guard<std::recursive_mutex> lck_append(log_mtx);
    if (getLength() == 0) {
        initCompress(compress);
    }

    size_t free_size = emptySize();
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
    setLength(getLength());
    return write_size;
}

void Buffer::release() {
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

size_t Buffer::emptySize() {
    return buffer_size - (write_ptr - buffer_ptr);
}

char *Buffer::getLogPath() {
    return buffer_header.getLogPath();
}

void Buffer::setFileFlush(FileFlush *flush) {
    file_flush_ptr = flush;
}

void Buffer::callFileFlush() {
    callFileFlush(file_flush_ptr);
}

void Buffer::callFileFlush(FileFlush *flush) {
    callFileFlush(file_flush_ptr, nullptr);
}

void Buffer::callFileFlush(FileFlush *flush, Buffer *buffer) {
    if (flush == nullptr) {
        if (buffer != nullptr) {
            delete buffer;
        }
        return;
    }

    std::lock_guard<std::recursive_mutex> lck_flush(log_mtx);
    if (getLength() > 0) {
        if (compress && Z_NULL != zStream.state) {
            deflateEnd(&zStream);
        }
        auto *buffer_flush = new BufferFlush(log_file_ptr);
        buffer_flush->write(data_ptr, getLength());
        buffer_flush->releaseThiz(buffer);
        clear();
        flush->asyncFlush(buffer_flush);
    } else {
        delete buffer;
    }
}

void Buffer::changeLogPath(char *path) {
    if (log_file_ptr != nullptr) {
        callFileFlush();
    }
    initData(path, strlen(path), compress);
}

void Buffer::clear() {
    std::lock_guard<std::recursive_mutex> lck_clear(log_mtx);
    write_ptr = data_ptr;
    memset(write_ptr, '\0', emptySize());
    setLength(getLength());
}

bool Buffer::initCompress(bool is_compress) {
    compress = is_compress;
    if (compress) {
        zStream.zalloc = Z_NULL;
        zStream.zfree = Z_NULL;
        zStream.opaque = Z_NULL;
        return Z_OK == deflateInit2(&zStream, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    }
    return false;
}

bool Buffer::openLogFile(const char *path) {
    if (path != nullptr) {
        FILE *file = fopen(path, "ab+");
        if (file != nullptr) {
            log_file_ptr = file;
            return true;
        }
    }
    return false;
}
