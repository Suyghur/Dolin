//
// Created by #Suyghur, on 2021/08/20.
//

#ifndef DOLIN_HAWKEYE_SIGNAL_UTILS_H
#define DOLIN_HAWKEYE_SIGNAL_UTILS_H

#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*hawkeye_signal_handler_function)(int, struct siginfo *, void *);

class SignalUtils {
public:
    static bool HasSiAddr(int si_signo, int si_code);

    static const char *GetSigName(int sig);

    static const char *GetSigCode(int signo, int code);

    static bool RegisterSignalHandler(hawkeye_signal_handler_function handler, struct sigaction old_handlers[NSIG]);

    static void UnregisterSignalHandler(struct sigaction old_handlers[NSIG]);
};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_HAWKEYE_SIGNAL_UTILS_H
