//
// Created by #Suyghur, on 2021/08/09.
//

#ifndef DOLIN_HAWKEYE_MMAP_BUFFER_H
#define DOLIN_HAWKEYE_MMAP_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

class MmapBuffer {
public:

    MmapBuffer();

    ~MmapBuffer();

    void WriteBuffer(const char *path, const char *content);

    void CloseBuffer();

private:
    int content_len = 0;

    int buffer_file_fd = -1;

    long *buffer_ptr = nullptr;

};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_HAWKEYE_MMAP_BUFFER_H
