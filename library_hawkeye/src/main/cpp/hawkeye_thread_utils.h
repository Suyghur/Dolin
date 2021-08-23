//
// Created by #Suyghur, on 2021/08/19.
//

#ifndef DOLIN_HAWKEYE_THREAD_UTILS_H
#define DOLIN_HAWKEYE_THREAD_UTILS_H

#include <cstdio>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

class ThreadUtils {
public:
    static size_t GetThreads(pid_t pid, pid_t *out, size_t size);
};

#ifdef __cplusplus
}
#endif


#endif //DOLIN_HAWKEYE_THREAD_UTILS_H
