//
// Created by #Suyghur, on 2021/4/7.
//

#include "buffer_header.h"
#include "../kit/common_log.h"

dolin_common::BufferHeader::BufferHeader(void *data, size_t size) : data_ptr((char *) data), data_size(size) {}

dolin_common::BufferHeader::~BufferHeader() = default;

void dolin_common::BufferHeader::InitHeader(dolin_common::Header &header) {
    if ((sizeof(char) + sizeof(size_t) + sizeof(size_t) + header.log_path_len) > data_size) {
        return;
    }
    memcpy(data_ptr, &header.magic, sizeof(char));
    //log长度
    memcpy(data_ptr + sizeof(char), &header.log_len, sizeof(size_t));
    //log路径长度
    memcpy(data_ptr + sizeof(char) + sizeof(size_t), &header.log_path_len, sizeof(size_t));
    //log路径
    memcpy(data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t), header.log_path, header.log_path_len);
    //限制大小
    memcpy(data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + header.log_path_len, &header.limit_size, sizeof(size_t));
    //是否压缩
    char compress = 0;
    if (header.compress) {
        compress = 1;
    }
    memcpy(data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + header.log_path_len + sizeof(size_t) + sizeof(size_t), &compress, sizeof(char));

}

/**
 * 获取原始头（不加长度）
 */
void *dolin_common::BufferHeader::GetOriginPtr() {
    return data_ptr;
}

/**
 * 获取头部信息（包含头信息长度）
 */
void *dolin_common::BufferHeader::GetDataPtr() {
    return data_ptr + GetHeaderLen();
}

/**
 * 获取写入信息（包含头信息长度和日志长度）
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

        size_t limit_size = 0;
        memcpy(&limit_size, data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + log_path_len, sizeof(size_t));
        header->limit_size = limit_size;

        char compress = (data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + log_path_len + sizeof(size_t))[0];
        header->compress = compress == 1;


        LOGD("JNI-> log_len : %d", log_len);
        LOGD("JNI-> log_path_len : %d", log_path_len);
        LOGD("JNI-> log_path : %s", log_path);
        LOGD("JNI-> limit_size : %d", limit_size);
        LOGD("JNI-> compress : %c", compress);
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


size_t dolin_common::BufferHeader::GetLimitSize() {
    size_t size = 0;
    if (IsAvailable()) {
        memcpy(&size, data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + GetLogPathLen(), sizeof(size_t));
    }
    return size;
}


bool dolin_common::BufferHeader::IsCompress() {
    if (IsAvailable()) {
        return ((data_ptr + sizeof(char) + sizeof(size_t) + sizeof(size_t) + GetLogPathLen() + sizeof(size_t))[0]) == 1;
    }
    return false;
}

bool dolin_common::BufferHeader::IsAvailable() {
    return data_ptr[0] == kMagicHeader;
}

size_t dolin_common::BufferHeader::CalculateHeaderLen(size_t path_len) {
    return sizeof(char) + sizeof(size_t) + sizeof(size_t) + path_len + sizeof(size_t) + sizeof(char);
}




