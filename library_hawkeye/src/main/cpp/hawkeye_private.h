//
// Created by #Suyghur, on 2021/08/18.
//

#ifndef DOLIN_HAWKEYE_PRIVATE_H
#define DOLIN_HAWKEYE_PRIVATE_H

#include <signal.h>
#include <ucontext.h>
#include "hawkeye_sizeof_array.h"

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
static const int NUM_SIGNALS_TO_CATCH = sizeof_array(SIGNALS_TO_CATCH);

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

typedef void (*unwind_func_ptr)(int log_file, pid_t tid, struct ucontext *ctx, void *data);

typedef void (*unwinder_release_func_ptr)(void *data);

#ifndef DOLIN_HAWKEYE_MAX_FRAMES
#define HAWKEYE_FRAMES 128
#endif

#ifndef DOLIN_HAWKEYE_MAX_FUNCTION_NAME_LENGTH
#define HAWKEYE_MAX_FUNCTION_NAME_LENGTH 128
#endif

#endif //DOLIN_HAWKEYE_PRIVATE_H
