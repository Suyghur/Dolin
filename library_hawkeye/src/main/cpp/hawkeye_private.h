//
// Created by #Suyghur, on 2021/08/18.
//

#ifndef DOLIN_HAWKEYE_PRIVATE_H
#define DOLIN_HAWKEYE_PRIVATE_H

#include <signal.h>
#include <ucontext.h>
#include <linux/un.h>
#include "guard/mmap_guard.h"

#define HAWKEYE_MAX_FRAMES 128
#define HAWKEYE_MAX_FUNCTION_NAME_LENGTH 128
#define SEP_HEAD "*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***\n"
#define SEP_OTHER_INFO "--- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---\n"

/// Helper macro to get a size of array in elements count.
#define SIZEOF_ARRAY(array) sizeof(array)/sizeof(array[0])

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


typedef void *(*unwinder_init_func_ptr)(pid_t pid);

typedef void (*unwinder_release_func_ptr)(void *data);

typedef void (*unwinder_func_ptr)(MmapGuard *mmap_ptr, pid_t tid, struct ucontext *context, void *data);


#if defined(__cplusplus)
extern "C" {
#endif

void FillSockAddr(const char *socket_name, struct sockaddr_un *out_address);

size_t GetThreads(pid_t pid, pid_t *out, size_t size);


#if defined(__cplusplus)
}
#endif

#endif //DOLIN_HAWKEYE_PRIVATE_H
