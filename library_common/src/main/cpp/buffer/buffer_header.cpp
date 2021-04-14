//
// Created by #Suyghur, on 4/7/21.
//

#include "buffer_header.h"

dolin_common::BufferHeader::BufferHeader(void *data, size_t size) : data_ptr((char *) data), data_size(size) {}

dolin_common::BufferHeader::~BufferHeader() = default;

void dolin_common::BufferHeader::InitHeader(dolin_common::Header &header) {
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
void *dolin_common::BufferHeader::GetOriginPtr() {
    return data_ptr;
}

/**
 * 获取当前锚点
 */
void *dolin_common::BufferHeader::GetPtr() {
    return data_ptr + GetHeaderLen();
}

/**
 * 获取写入的锚点
 */
void *dolin_common::BufferHeader::GetWritePtr() {
    return data_ptr + GetHeaderLen() + GetLogLen();
}

dolin_common::Header *dolin_common::BufferHeader::GetHeader() {
    auto *header = new Header();
    if (IsAvailable()) {
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

size_t dolin_common::BufferHeader::GetHeaderLen() {
    if (IsAvailable()) {
        return CalculateHeaderLen(GetLogPathLen());
    }
    return 0;
}

void dolin_common::BufferHeader::SetLogLen(size_t len) {
    if (IsAvailable()) {
        memcpy(data_ptr + sizeof(char), &len, sizeof(size_t));
    }
}

size_t dolin_common::BufferHeader::GetLogLen() {
    if (IsAvailable()) {
        size_t len = 0;
        memcpy(&len, data_ptr + sizeof(char), sizeof(size_t));
        //log长度总是大于0并小于buffer_size减去header长度的
        if (len > 0 && len < (data_size - GetHeaderLen())) {
            return len;
        }
    }
    return 0;
}

size_t dolin_common::BufferHeader::GetLogPathLen() {
    if (IsAvailable()) {
        size_t len = 0;
        memcpy(&len, data_ptr + sizeof(char) + sizeof(size_t), sizeof(size_t));
        //log path 的长度不能大于整个buffer减去header中其他数据的长度
        if (len > 0 && len < data_size - CalculateHeaderLen(0)) {
            return len;
        }
    }
    return 0;
}

char *dolin_common::BufferHeader::GetLogPath() {
    if (IsAvailable()) {
        size_t log_path_len = GetLogPathLen();
        if (log_path_len > 0) {
            char *log_path = new char[log_path_len + 1];
            memset(log_path, 0, log_path_len + 1);
            memcpy(log_path, data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t), log_path_len);
            return log_path;
        }
    }
    return nullptr;
}


bool dolin_common::BufferHeader::IsCompress() {
    if (IsAvailable()) {
        return ((data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + GetLogPathLen())[0]) == 1;
    }
    return false;
}

bool dolin_common::BufferHeader::IsAvailable() {
    return data_ptr[0] == kMagicHeader;
}

size_t dolin_common::BufferHeader::CalculateHeaderLen(size_t path_len) {
    return sizeof(char) + sizeof(size_t) + sizeof(size_t) + path_len + sizeof(char);
}




