//
// Created by #Suyghur, on 4/7/21.
//

#include "buffer_header.h"

buffer_header::BufferHeader::BufferHeader(void *data, size_t size) : data_ptr((char *) data), data_size(size) {}

buffer_header::BufferHeader::~BufferHeader() = default;

void buffer_header::BufferHeader::initHeader(buffer_header::Header &header) {
    if ((sizeof(char) + sizeof(size_t) + sizeof(size_t) + header.log_path_len) > data_size) {
        return;
    }
    memcpy(data_ptr, &header.magic, sizeof(char));
    memcpy(data_ptr + sizeof(char), &header.log_len, sizeof(size_t));
    memcpy(data_ptr + sizeof(char) + sizeof(size_t), &header.log_path_len, sizeof(size_t));
    memcpy(data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t), header.log_path, header.log_path_len);
    char compress = 0;
    if (header.compress) {
        compress = 1;
    }
    memcpy(data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + header.log_path_len, &compress, sizeof(char));
}

/**
 * 获取原始的锚点
 */
void *buffer_header::BufferHeader::getOriginPtr() {
    return data_ptr;
}

/**
 * 获取当前锚点
 */
void *buffer_header::BufferHeader::getPtr() {
    return data_ptr + getHeaderLen();
}

/**
 * 获取写入的锚点
 */
void *buffer_header::BufferHeader::getWritePtr() {
    return data_ptr + getHeaderLen() + getLogLen();
}

buffer_header::Header *buffer_header::BufferHeader::getHeader() {
    auto *header = new Header();
    if (isAvailable()) {
        header->magic = kMagicHeader;

        size_t log_len = 0;
        memcpy(&log_len, data_ptr + sizeof(char), sizeof(size_t));
        header->log_len = log_len;

        size_t log_path_len = 0;
        memcpy(&log_path_len, data_ptr + sizeof(char) + sizeof(size_t), sizeof(size_t));
        header->log_path_len = log_path_len;

        char *log_path = new char[log_path_len + 1];
        memset(log_path, 0, log_path_len + 1);
        memcpy(log_path, data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t), log_path_len);
        header->log_path = log_path;

        char compress = (data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + log_path_len)[0];
        header->compress = compress == 1;
    }
    return header;
}

size_t buffer_header::BufferHeader::getHeaderLen() {
    if (isAvailable()) {
        return calculateHeaderLen(getLogPathLen());
    }
    return 0;
}

void buffer_header::BufferHeader::setLogLen(size_t len) {
    if (isAvailable()) {
        memcpy(data_ptr + sizeof(char), &len, sizeof(size_t));
    }
}

size_t buffer_header::BufferHeader::getLogLen() {
    if (isAvailable()) {
        size_t len = 0;
        memcpy(&len, data_ptr + sizeof(char), sizeof(size_t));
        //log长度总是大于0并小于buffer_size减去header长度的
        if (len > 0 && len < (data_size - getHeaderLen())) {
            return len;
        }
    }
    return 0;
}

size_t buffer_header::BufferHeader::getLogPathLen() {
    if (isAvailable()) {
        size_t len = 0;
        memcpy(&len, data_ptr + sizeof(char) + sizeof(size_t), sizeof(size_t));
        //log path 的长度不能大于整个buffer减去header中其他数据的长度
        if (len > 0 && len < data_size - calculateHeaderLen(0)) {
            return len;
        }
    }
    return 0;
}

char *buffer_header::BufferHeader::getLogPath() {
    if (isAvailable()) {
        size_t log_path_len = getLogPathLen();
        if (log_path_len > 0) {
            char *log_path = new char[log_path_len + 1];
            memset(log_path, 0, log_path_len + 1);
            memcpy(log_path, data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t), log_path_len);
            return log_path;
        }
    }
    return nullptr;
}


bool buffer_header::BufferHeader::isCompress() {
    if (isAvailable()) {
        return ((data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + getLogPathLen())[0]) == 1;
    }
    return false;
}

bool buffer_header::BufferHeader::isAvailable() {
    return data_ptr[0] == kMagicHeader;
}

size_t buffer_header::BufferHeader::calculateHeaderLen(size_t path_len) {
    return sizeof(char) + sizeof(size_t) + sizeof(size_t) + path_len + sizeof(char);
}




