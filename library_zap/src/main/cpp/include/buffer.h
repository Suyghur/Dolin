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

using namespace zap;

class Buffer {
public:

    bool map_buffer = true;

    Buffer(char *ptr, size_t buffer_size);

    ~Buffer();

    void InitData(char *log_path, size_t log_path_len, bool _compress);

    size_t GetLength();

    size_t Append(const char *log, size_t len);

    void Release();

    size_t EmptySize();

    char *GetLogPath();

    void SetFileFlush(FileFlush *flush);

    void CallFileFlush();

    void CallFileFlush(FileFlush *flush);

    void CallFileFlush(FileFlush *flush, Buffer *buffer);

    void ChangeLogPath(char *path);

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

    void Clear();

    void SetLength(size_t len);

    bool InitCompress(bool compress);

    bool OpenLogFile(const char *path);


};


#endif //DOLIN_ZAP_BUFFER_H
