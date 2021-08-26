//
// Created by #Suyghur, on 2021/08/20.
//

#ifndef DOLIN_HAWKEYE_DUMPER_UTILS_H
#define DOLIN_HAWKEYE_DUMPER_UTILS_H

#include <sys/types.h>
#include <sys/ucontext.h>
#include <stdint.h>
#include "hawkeye_mmap_buffer.h"

#define SEP_HEAD "*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***\n"
#define SEP_OTHER_INFO "--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---\n"

#ifdef __cplusplus
extern "C" {
#endif

class DumperUtils {
public:
    static int DumpCreateFile(const char *path);

    static void Record2Buffer(MmapBuffer *mmap_ptr, const char *format, ...);

    static void DumpWriteLine(int log_fd, const char *format, ...);

//    static void DumpHeader(int log_fd, pid_t pid, pid_t tid, int signo, int si_code, void *falutaddr, struct ucontext *context);

    static void DumpHeader( int log_fd, pid_t pid, pid_t tid, int signo, int si_code, void *falutaddr, struct ucontext *context);

    static void DumpOtherThreadHeader(int log_fd, pid_t pid, pid_t tid);

    static void DumpBacktraceLine(int log_fd, int counter, intptr_t pc, const char *map_name, const char *func_name, intptr_t func_offset);

private:
    static ssize_t __ReadFile(const char *file_name, char *out_buffer, size_t buffer_size);

    static void __DumpOtherThreadRegistersByPtrace(int log_fd, pid_t tid);

    static void __WriteProcessAndThreadInfo(int log_fd, pid_t pid, pid_t tid, char *process_name_buffer, size_t process_name_buffer_size);

    static void __DumpSignalInfo(int log_fd, int signo, int si_code, void *faultaddr, char *str_buffer, size_t str_buffer_size);
};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_HAWKEYE_DUMPER_UTILS_H
