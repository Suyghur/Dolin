//
// Created by #Suyghur, on 2021/08/18.
//

#include "hawkeye_core.h"
#include "hawkeye_private.h"
#include "hawkeye_log.h"
#include "hawkeye_signal_utils.h"
#include <csignal>
#include <malloc.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <android/log.h>
#include <sys/socket.h>
#include <linux/un.h>
#include <cstring>
#include <cerrno>
#include <linux/prctl.h>
#include <sys/prctl.h>


struct hawkeye_core_context *core_context = nullptr;

bool HawkeyeCore::InitCore(const char *socket_name) {
    if (core_context) {
        LOGE("Hawkeye core is already initialized.");
        return false;
    }

    // socket name can't be null or empty.
    if (!socket_name || !*socket_name) {
        LOGE("socket name can't be null or empty.");
        return false;
    }

    // initializing context instance.
    core_context = (struct hawkeye_core_context *) malloc(sizeof(struct hawkeye_core_context));
    memset(core_context, 0, sizeof(struct hawkeye_core_context));

    // saving old dumpable flag, not checking for error.
    core_context->old_dumpable = prctl(PR_GET_DUMPABLE);

    // setting dumpable flag, required for ptrace.
    prctl(PR_SET_DUMPABLE, 1);

    // filling in socket address.
    FillSockAddr(socket_name, &core_context->socket_address);

    // trying to register signal handler.
    if (!SignalUtils::RegisterSignalHandler(&__SignalHandler, core_context->old_handlers)) {
        ReleaseCore();
        return false;
    }
    return true;
}

bool HawkeyeCore::ReleaseCore() {
    if (!core_context) {
        return false;
    }

    // restoring old signal handlers.
    SignalUtils::UnregisterSignalHandler(core_context->old_handlers);

    // restoring old dumpable state, note tah PR_GET_DUMPABLE might fail.
    if (core_context->old_dumpable >= 0) {
        prctl(PR_SET_DUMPABLE, core_context->old_dumpable);
    }

    // freeing memory.
    free(core_context);
    core_context = nullptr;
    return true;
}

/// signal handler.
void HawkeyeCore::__SignalHandler(int signo, struct siginfo *siginfo, void *ctx) {
    // restoring an old handler to make built-in Android crash mechanism work.
    sigaction(signo, &core_context->old_handlers[signo], nullptr);

    // filling message fields.
    struct hawkeye_crash_message msg{};
    msg.pid = getpid();
    msg.tid = gettid();
    msg.signo = signo;
    msg.si_code = siginfo->si_code;
    msg.fault_addr = siginfo->si_addr;
    memcpy(&msg.context, ctx, sizeof(struct ucontext));
    LOGE("Signal caught: %d (%s), code %d (%s) pid: %d, tid: %d", signo, SignalUtils::GetSigName(signo), siginfo->si_code,
         SignalUtils::GetSigCode(signo, siginfo->si_code), msg.pid, msg.tid);

    // connecting to service using UNIX domain socket, sending message to it.
    // using blocking sockets.
    const int sock = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (sock < 0) {
        LOGE("Couldn't create socket, error: %s (%d)", strerror(errno), errno);
    } else {
        // connecting.
        if (connect(sock, (struct sockaddr *) &core_context->socket_address, sizeof(struct sockaddr_un))) {
            LOGE("Couldn't connect socket, error: %s (%d)", strerror(errno), errno);
        } else {
            // sending
            const ssize_t sent = send(sock, &msg, sizeof(msg), MSG_NOSIGNAL);
            if (sent < 0) {
                LOGE("Send error: %s (%d)", strerror(errno), errno);
            } else if (sent != sizeof(msg)) {
                LOGE("Error: couldn't send whole message, sent bytes: %d, message size: %d", (int) sent, (int) sizeof(msg));
            } else {
                LOGD("Successfully sent data to crash service.");
            }

            // blocking read.
            char c = 0;
            if (recv(sock, &c, 1, MSG_NOSIGNAL) < 0) {
                LOGE("Recv error: %s (%d)", strerror(errno), errno);
            }
        }
        // closing a socket.
        close(sock);
    }

    // in some cases we need to re-send a signal to run standard bionic handler.
    if (siginfo->si_code <= 0 || signo == SIGABRT) {
        if (syscall(__NR_tgkill, getpid(), gettid(), signo) < 0) {
            _exit(1);
        }
    }
}

