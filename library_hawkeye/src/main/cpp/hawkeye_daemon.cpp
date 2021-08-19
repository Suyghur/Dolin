//
// Created by #Suyghur, on 2021/08/18.
//

#include "hawkeye_daemon.h"
#include "log.h"
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

bool Daemon::PtraceAttach(pid_t tid) {
    if (ptrace(PTRACE_ATTACH, tid, nullptr, nullptr) < 0) {
        LOGE("Ptrace attach failed to tid: %d errno: %d (%s)", (int) tid, errno, strerror(errno));
        return false;
    }
    int wait_status = 0;
    if (waitpid(tid, &wait_status, 0) < 0) {
        LOGE("Waitpid failed to tid: %d errno: %d (%s)", (int) tid, errno, strerror(errno));
        return false;
    }

    if (!WIFSTOPPED(wait_status)) {
        LOGE("Thread %d isn't stopped, status: %d", (int) tid, wait_status);
        return false;
    }
    return true;
}

void Daemon::PtraceDetach(pid_t tid) {
    if (ptrace(PTRACE_DETACH, tid, nullptr, nullptr) < 0) {
        LOGE("Ptrace detach failed from tid: %d errno: %d (%s)", (int) tid, errno, strerror(errno));
    }
}

bool Daemon::DaemonCreateReport(struct hawkeye_crash_message *message) {
    if (!PtraceAttach(message->tid)) {
        return false;
    }
    pid_t tids[64];
//    const size_t tids_size=
    return false;
}

void Daemon::DaemonProcessClient(int client_socket) {

}

void *Daemon::DaemonFunction(void *args) {
    return nullptr;
}

bool Daemon::StartDaemon(const char *socket_name, const char *log_file, daemon_start_stop_callback start_callback, daemon_crash_callback crash_callback,
                         daemon_start_stop_callback stop_callback) {
    return false;
}

bool Daemon::StopDaemon() {
    return false;
}

void *Daemon::GetDaemonCallbacksArg() {
    return nullptr;
}

