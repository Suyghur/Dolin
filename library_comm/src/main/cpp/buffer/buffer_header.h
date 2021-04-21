//
// Created by #Suyghur, on 2021/4/7.
//

#ifndef DOLIN_COMMON_BUFFER_HEADER_H
#define DOLIN_COMMON_BUFFER_HEADER_H

#include <string>

namespace dolin_common {
    static const char kMagicHeader = '\x11';

    struct Header {
        char magic;
        size_t log_len;
        size_t log_path_len;
        char *log_path;
        bool compress;
        size_t limit_size;
    };

    class BufferHeader {
    public:
        BufferHeader(void *data, size_t size);

        ~BufferHeader();

        void InitHeader(Header &header);

        void *GetOriginPtr();

        void *GetDataPtr();

        void *GetWritePtr();

        Header *GetHeader();

        size_t GetHeaderLen();

        void SetLogLen(size_t len);

        size_t GetLogLen();

        size_t GetLogPathLen();

        char *GetLogPath();

        size_t GetLimitSize();

        bool IsCompress();

        bool IsAvailable();

        static size_t CalculateHeaderLen(size_t path_len);

    private:
        char *data_ptr;
        size_t data_size;
    };
}


#endif //DOLIN_COMMON_BUFFER_HEADER_H
