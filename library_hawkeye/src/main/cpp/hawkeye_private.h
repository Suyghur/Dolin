//
// Created by #Suyghur, on 2021/08/18.
//

#ifndef DOLIN_HAWKEYE_PRIVATE_H
#define DOLIN_HAWKEYE_PRIVATE_H

#include <signal.h>
#include <ucontext.h>
#include <linux/un.h>
#include <fcntl.h>
#include <cerrno>
#include <sys/syscall.h>
#include "guard/mmap_guard.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define HAWKEYE_MAX_FRAMES 128
#define HAWKEYE_MAX_FUNCTION_NAME_LENGTH 128
#define SEP_HEAD "*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***"
#define SEP_OTHER_INFO "--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---"
#define FULL_VERSION "hawkeye_1.0.0"
#define SCENE_TYPE "native"

/// Helper macro to get a size of array in elements count.
#define SIZEOF_ARRAY(array) sizeof(array)/sizeof(array[0])

#define HAWKEYE_LOG_BUFFER_SIZE (1024 * sizeof(char))

#define HAWKEYE_TEMP_FAILURE_RETRY(exp) ({             \
            __typeof__(exp) _rc;                       \
            do {                                       \
                errno = 0;                             \
                _rc = (exp);                           \
            } while (_rc == -1 && errno == EINTR);     \
            _rc; })

#ifndef __LP64__
#define HAWKEYE_SYSCALL_GETDENTS SYS_getdents
#else
#define HAWKEYE_SYSCALL_GETDENTS SYS_getdents64
#endif

/// Array of constants with signal numbers to catch.
static const int SIGNALS_TO_CATCH[] = {
        SIGABRT,
        SIGBUS,
        SIGFPE,
        SIGSEGV,
        SIGILL,
#if defined(SIGSTKFLT)
        SIGSTKFLT,
#endif
        SIGTRAP,
};

/// Count of signals to catch
static const int NUM_SIGNALS_TO_CATCH = SIZEOF_ARRAY(SIGNALS_TO_CATCH);

/// Struct for message that is sent from signal handler to daemon
struct hawkeye_crash_message {
    pid_t pid;
    pid_t tid;
    int signo;
    int si_code;
    void *fault_addr;
    struct ucontext context;
};

typedef struct {
#ifndef __LP64__
    unsigned long d_ino;
    unsigned long d_off;
    unsigned short d_reclen;
#else
    ino64_t d_ino;
    off64_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
#endif
    char d_name[1];
} hawkeye_dirent_t;

typedef void *(*unwinder_init_func_ptr)(pid_t pid);

typedef void (*unwinder_release_func_ptr)(void *data);

typedef void (*unwinder_func_ptr)(MmapGuard *mmap_ptr, pid_t tid, struct ucontext *context, void *data);

void Stamp2Standard(long ts, char *date, size_t len);

void RecordNewline(MmapGuard *mmap_ptr, const char *format, ...);

void RecordLine(MmapGuard *mmap_ptr, const char *format, ...);

void FillSockAddr(const char *socket_name, struct sockaddr_un *out_address);

size_t GetThreads(pid_t pid, pid_t *out, size_t size);

void GetABIs(char *buf, size_t len);

void GetKernelVersion(char *buf, size_t len);

void GetLogcatInfo(MmapGuard *mmap_ptr, pid_t pid, const char *stack_name, size_t lines, char priority);

void GetFds(MmapGuard *mmap_ptr, pid_t pid);

void GetNetworkInfo(MmapGuard *mmap_ptr, pid_t pid);

#if defined(__cplusplus)
}
#endif

#endif //DOLIN_HAWKEYE_PRIVATE_H
