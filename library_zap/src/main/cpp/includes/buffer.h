//
// Created by #Suyghur, on 4/7/21.
//

#ifndef DOLIN_ZAP_BUFFER_H
#define DOLIN_ZAP_BUFFER_H

#include <string>
#include <mutex>
#include <zlib.h>
#include <sys/mman.h>
#include "buffer_header.h"
#include "file_flush.h"

using namespace buffer_header;

class Buffer {
public:

    bool map_buffer = true;

    Buffer(char *ptr, size_t buffer_size);

    ~Buffer();

    void initData(char *log_path, size_t log_path_len, bool compress);

    size_t getLength();

    size_t append(const char *log, size_t len);

    void release();

    size_t emptySize();

    char *getLogPath();

    void setFileFlush(FileFlush *flush);

    void callFileFlush();

    void callFileFlush(FileFlush *flush);

    void callFileFlush(FileFlush *flush, Buffer *buffer);

    void changeLogPath(char *path);

private:
    FILE *log_file_ptr = nullptr;
    FileFlush *file_flush_ptr = nullptr;
    char *const buffer_ptr = nullptr;
    char *data_ptr = nullptr;
    char *write_ptr = nullptr;

    size_t buffer_size = 0;
    std::recursive_mutex log_mtx;

    BufferHeader buffer_header;
    z_stream zStream{};
    bool compress = false;

    void clear();

    void setLength(size_t len);

    bool initCompress(bool compress);

    bool openLogFile(const char *path);


};


#endif //DOLIN_ZAP_BUFFER_H
