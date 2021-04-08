//
// Created by #Suyghur, on 4/7/21.
//

#ifndef DOLIN_BUFFER_HEADER_H
#define DOLIN_BUFFER_HEADER_H

#include <string>

namespace buffer_header {
    static const char kMagicHeader = '\x11';

    struct Header {
        char magic;
        size_t log_len;
        size_t log_path_len;
        char *log_path;
        bool compress;
    };

    class BufferHeader {
    public:
        BufferHeader(void *data, size_t size);

        ~BufferHeader();

        void initHeader(Header &header);

        void *getOriginPtr();

        void *getPtr();

        void *getWritePtr();

        Header *getHeader();

        size_t getHeaderLen();

        void setLogLen(size_t len);

        size_t getLogLen();

        size_t getLogPathLen();

        char *getLogPath();

        bool isCompress();

        bool isAvailable();

        static size_t calculateHeaderLen(size_t path_len);

    private:
        char *data_ptr;
        size_t data_size;
    };
}


#endif //DOLIN_BUFFER_HEADER_H
