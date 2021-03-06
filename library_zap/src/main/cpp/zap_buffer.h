//
// Created by #Suyghur, on 2021/4/7.
//

#ifndef DOLIN_ZAP__BUFFER_H
#define DOLIN_ZAP_BUFFER_H


#include <string>
#include <mutex>
#include <zlib.h>
#include <sys/mman.h>
#include "zap_buffer_header.h"
#include "zap_file_flush.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace dolin_zap;

class Buffer {
public:

    bool map_buffer = true;

    Buffer(char *ptr, size_t buffer_size);

    ~Buffer();

    void InitData(char *log_path, size_t log_path_len, size_t limit_size, bool _compress);

    size_t GetLength();

    size_t Append(const char *log, size_t len);

    void Release();

    size_t EmptySize();

    char *GetLogPath();

    void SetFileFlush(FileFlush *flush);

    void CallFileFlush();

    void CallFileFlush(FileFlush *flush);

    void CallFileFlush(FileFlush *flush, Buffer *buffer);

    void ExpLogPath(char *path, size_t limit_size);

    bool IsCurrentLogFileOversize();

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
//    size_t part_num = 1;
//    size_t limit_size = 0;
    bool compress = false;

    void Clear();

    void SetLength(size_t len);

    bool InitCompress(bool _compress);

    bool OpenLogFile(const char *path);

    size_t GetCurrentLogFileSize();

};

#ifdef __cplusplus
}
#endif

#endif //DOLIN__BUFFER_H
