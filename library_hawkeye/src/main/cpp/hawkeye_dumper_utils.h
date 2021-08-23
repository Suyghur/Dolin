//
// Created by #Suyghur, on 2021/08/20.
//

#ifndef DOLIN_HAWKEYE_DUMPER_UTILS_H
#define DOLIN_HAWKEYE_DUMPER_UTILS_H

#include <sys/types.h>
#include <sys/ucontext.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

class DumperUtils {
public:
    static int DumpCreateFile(const char *path);

    static void DumpWriteLine(int fd, const char *format, ...);

    static void DumpHeader(int out_file, pid_t pid, pid_t tid, int signo, int si_code, void *falutaddr, struct ucontext *context);

    static void DumpOtherThreadHeader(int out_file, pid_t pid, pid_t tid);

    static void DumpBacktraceLine(int out_file, int counter, intptr_t pc, const char *map_name, const char *func_name, intptr_t func_offset);

private:
    static ssize_t __ReadFile(const char *file_name, char *out_buffer, size_t buffer_size);

    static void __DumpOtherThreadRegistersByPtrace(int out_file, pid_t tid);

    static void __WriteProcessAndThreadInfo(int out_file, pid_t pid, pid_t tid, char *process_name_buffer, size_t process_name_buffer_size);

    static void __DumpSignalInfo(int ouf_file, int signo, int si_code, void *faultaddr, char *str_buffer, size_t str_buffer_size);
};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_HAWKEYE_DUMPER_UTILS_H
