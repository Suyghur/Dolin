//
// Created by #Suyghur, on 2021/08/18.
//

#ifndef DOLIN_HAWKEYE_CORE_H
#define DOLIN_HAWKEYE_CORE_H

#include <signal.h>
#include <sys/socket.h>
#include <linux/un.h>

#ifdef __cplusplus
extern "C" {
#endif

struct hawkeye_core_context {
    // Old handlers of signals that we restore on de-initialization. Keep values for all possible
    // signals, for unused signals NULL value is stored.
    struct sigaction old_handlers[NSIG];

    // Socket address that we use to communicate with debugger.
    struct sockaddr_un socket_address;

    // Old state of dumpable flag. Restoring it when a signal handler is de-initialized.
    int old_dumpable;
};

void PerformSignalCallback(int signo, struct siginfo *siginfo, void *ctx);


#ifdef __cplusplus
}
#endif
#endif //DOLIN_HAWKEYE_CORE_H
