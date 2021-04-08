//
// Created by #Suyghur, on 4/7/21.
//

#ifndef DOLIN_ZAP_BUFFER_FLUSH_H
#define DOLIN_ZAP_BUFFER_FLUSH_H


#include <cstdio>

class BufferFlush {

public:
    BufferFlush(FILE *log_file, size_t size = 128);

    ~BufferFlush();

    void write(void *data, size_t len);

    void reset();

    size_t getLength();

    void *getPtr();

    FILE *getLogFile();

    void releaseThiz(void *buffer);

private:
    FILE *log_file_ptr = nullptr;
    void *release_ptr = nullptr;
    char *data_ptr = nullptr;
    char *write_ptr = nullptr;
    size_t capacity;

    size_t emptySize();
};


#endif //DOLIN_ZAP_BUFFER_FLUSH_H
