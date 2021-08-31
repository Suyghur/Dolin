//
// Created by #Suyghur, on 2021/08/20.
//

#ifndef DOLIN_HAWKEYE_DUMPER_UTILS_H
#define DOLIN_HAWKEYE_DUMPER_UTILS_H

#include <sys/types.h>
#include <sys/ucontext.h>
#include <stdint.h>
#include "guard/mmap_guard.h"


#ifdef __cplusplus
extern "C" {
#endif

class DumperUtils {
public:

    static void DumpHeader(MmapGuard *mmap_ptr, pid_t pid, pid_t tid, int signo, int si_code, void *falutaddr, struct ucontext *context);

    static void DumpOtherThreadHeader(MmapGuard *mmap_ptr, pid_t pid, pid_t tid);

    static void DumpBacktraceLine(MmapGuard *mmap_ptr, int counter, intptr_t pc, const char *map_name, const char *func_name, intptr_t func_offset);

    static void DumpLogcatInfo(MmapGuard *mmap_ptr, pid_t pid);

    static void DumpFds(MmapGuard *mmap_ptr, pid_t pid);

private:
    static ssize_t ReadFile(const char *file_name, char *out_buffer, size_t buffer_size);

    static void DumpOtherThreadRegistersByPtrace(MmapGuard *mmap_ptr, pid_t tid);

    static void WriteProcessAndThreadInfo(MmapGuard *mmap_ptr, pid_t pid, pid_t tid, char *process_name_buffer, size_t process_name_buffer_size);

    static void DumpSignalInfo(MmapGuard *mmap_ptr, int signo, int si_code, void *faultaddr, char *str_buffer, size_t str_buffer_size);

};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_HAWKEYE_DUMPER_UTILS_H
