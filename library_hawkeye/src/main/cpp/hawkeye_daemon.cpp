//
// Created by #Suyghur, on 2021/08/18.
//

#include "hawkeye_daemon.h"
#include "hawkeye_log.h"
#include "hawkeye_thread_utils.h"
#include "hawkeye_dumper_utils.h"
#include "hawkeye_socket_utils.h"
#include "hawkeye_unwinder.h"
#include "hawkeye_fd_utils.h"
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cstring>
#include <cerrno>


struct hawkeye_daemon_context *daemon_context = nullptr;


bool HawkeyeDaemon::PtraceAttach(pid_t tid) {
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

void HawkeyeDaemon::PtraceDetach(pid_t tid) {
    if (ptrace(PTRACE_DETACH, tid, nullptr, nullptr) < 0) {
        LOGE("Ptrace detach failed from tid: %d errno: %d (%s)", (int) tid, errno, strerror(errno));
    }
}

bool HawkeyeDaemon::DaemonCreateReport(struct hawkeye_crash_message *message) {
    ///native-crash_00001629270036885000.crash
    if (!PtraceAttach(message->tid)) {
        return false;
    }
    pid_t tids[64];
    const size_t tids_size = ThreadUtils::GetThreads(message->pid, tids, SIZEOF_ARRAY(tids));
    for (pid_t *it = tids, *end = tids + tids_size; it != end; ++it) {
        // attaching errors for background threads are not fatal, just skipping such threads by setting tid to 0.
        if (!PtraceAttach(*it)) {
            *it = 0;
        }
    }
    int log_fd = -1;
    if (daemon_context->log_folder_path) {
        log_fd = DumperUtils::DumpCreateFile(daemon_context->log_folder_path);
    }

    // writing a crash dump header
    DumperUtils::DumpHeader(log_fd, message->pid, message->tid, message->signo, message->si_code, message->fault_addr, &message->context);

    // unwinder initialization, should be done before any thread unwinding.
    void *const unwinder_data = daemon_context->unwinder_init(message->tid);

    // stack unwinding for a main thread.
    daemon_context->unwinder_func(log_fd, message->tid, &message->context, unwinder_data);

    // processing other threads: printing a header and stack trace.
    for (pid_t *it = tids, *end = tids + tids_size; it != end; ++it) {
        // skipping threads failed to attach.
        if (!*it) {
            continue;
        }

        // writing other thread header.
        DumperUtils::DumpOtherThreadHeader(log_fd, message->pid, *it);

        // stack unwinding for a secondary thread.
        daemon_context->unwinder_func(log_fd, *it, nullptr, unwinder_data);
    }

    // unwinder de-initialization
    daemon_context->unwinder_release(unwinder_data);

    // final line of crash dump.
    DumperUtils::DumpWriteLine(log_fd, " ");

    // closing output file.
    if (log_fd >= 0) {
        // closing file
        close(log_fd);
    }

    // detaching from all threads.
    PtraceDetach(message->tid);

    for (pid_t *it = tids, *end = tids + tids_size; it != end; ++it) {
        if (!*it) {
            continue;
        }
        PtraceDetach(*it);
    }

    return log_fd >= 0;
}

void HawkeyeDaemon::DaemonProcessClient(int client_socket) {
    struct hawkeye_crash_message message = {0, 0};
    ssize_t overall_read = 0;
    do {
        fd_set flag;
        FD_ZERO(&flag);
        FD_SET(client_socket, &flag);
        const int select_result = select(MAX(client_socket, daemon_context->interrupter[0]) + 1, &flag, NULL, NULL, NULL);
        if (select_result < 0) {
            LOGE("Select on recv error: %s (%d)", strerror(errno), errno);
            close(client_socket);
            return;
        }
        if (FD_ISSET(daemon_context->interrupter[0], &flag)) {
            // interrupting by pipe.
            close(client_socket);
            return;
        }
        const ssize_t bytes_read = recv(client_socket, (char *) &message + overall_read, sizeof(struct hawkeye_crash_message) - overall_read, MSG_NOSIGNAL);
        if (bytes_read < 0) {
            LOGE("Recv error: %s (%d)", strerror(errno), errno);
            close(client_socket);
            return;
        }
        overall_read += bytes_read;
    } while (overall_read < sizeof(struct hawkeye_crash_message));

    LOGD("Client info received, pid: %d tid: %d", message.pid, message.tid);

    // creating a report.
    const bool report_file_created = DaemonCreateReport(&message);

    // writing 1 byte as response.
    write(client_socket, "\0", 1);

    // closing a connection.
    close(client_socket);

    // running successful unwinding callback if it's set. We do it after detaching and disconnecting
    // from crashing process because at this point it can terminate. A callback may perform some
    // long operation, for example, synchronous networking and we shouldn't allow any bad UX
    // with a hang of application. In modern Android service has "a window of several minutes in which
    // it is still allowed to create and use services" so it won't be a problem.
    if (report_file_created && daemon_context->crash_callback) {
        daemon_context->crash_callback(daemon_context->log_folder_path, daemon_context->callback_arg);
    }
}

void *HawkeyeDaemon::DaemonFunction(void *args) {
    // creating socket.
    const int listener = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (listener < 0) {
        LOGE("Couldn't create socket, error: %s (%d)", strerror(errno), errno);
        return nullptr;
    }

    // setting options.
    {
        int n = 1;
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n));
    }

    // binding to an address.
    if (bind(listener, (struct sockaddr *) &daemon_context->socket_address, sizeof(struct sockaddr_un)) < 0) {
        LOGE("Couldn't bind socket, error: %s (%d)", strerror(errno), errno);
        return nullptr;
    }

    // listening
    if (listen(listener, SOCKET_BACKLOG) < 0) {
        LOGE("Couldn't listen socket, error: %s (%d)", strerror(errno), errno);
        return nullptr;
    }

    LOGD("HawkeyeDaemon is successfully started, accepting connections...");
    if (daemon_context->start_callback) {
        daemon_context->start_callback(daemon_context->callback_arg);
    }

    // accepting connections in a cycle.
    for (;;) {
        fd_set flag;
        FD_ZERO(&flag);
        FD_SET(listener, &flag);
        FD_SET(daemon_context->interrupter[0], &flag);
        const int select_result = select(MAX(listener, daemon_context->interrupter[0]) + 1, &flag, nullptr, nullptr, nullptr);
        if (select_result < 0) {
            LOGE("Select on accept error: %s (%d)", strerror(errno), errno);
            break;
        }
        if (FD_ISSET(daemon_context->interrupter[0], &flag)) {
            // interrupting by pipe.
            break;
        }

        struct sockaddr_storage storage{};
        auto *addr = (struct sockaddr *) &storage;
        socklen_t storage_len = sizeof(storage);
        int client_socket = accept(listener, addr, &storage_len);
        if (client_socket == -1) {
            LOGE("Accept failed, error: %s (%d)", strerror(errno), errno);
            continue;
        }
        LOGD("Client connected, socket %d", client_socket);
        DaemonProcessClient(client_socket);
    }

    close(listener);

    if (daemon_context->stop_callback) {
        daemon_context->stop_callback(daemon_context->callback_arg);
    }

    return nullptr;
}

bool HawkeyeDaemon::StartDaemon(const char *socket_name, const char *log_folder_path, hawkeye_daemon_start_stop_callback start_callback,
                                hawkeye_daemon_crash_callback crash_callback, hawkeye_daemon_start_stop_callback stop_callback,
                                void *callback_arg) {
    if (daemon_context) {
        LOGE("HawkeyeDaemon is already initialized.");
        return false;
    }

    // socket name can't be null or empty.
    if (!socket_name || !*socket_name) {
        LOGE("Socket name can't be null or empty.");
        return false;
    }

    // creating a new struct instance.
    daemon_context = (struct hawkeye_daemon_context *) malloc(sizeof(struct hawkeye_daemon_context));
    memset(daemon_context, 0, sizeof(hawkeye_daemon_context));
    daemon_context->start_callback = start_callback;
    daemon_context->crash_callback = crash_callback;
    daemon_context->stop_callback = stop_callback;
    daemon_context->callback_arg = callback_arg;

    // filling in socket address.
    SocketUtils::FillSockAddr(socket_name, &daemon_context->socket_address);

    // checking if unwinder is supported. setting unwind function.
    daemon_context->unwinder_init = &Unwinder::InitUnwinder;
    daemon_context->unwinder_release = &Unwinder::ReleaseUnwinder;
    daemon_context->unwinder_func = &Unwinder::DoUnwind;

    // copying log file path if set.
    if (log_folder_path) {
        size_t path_len = strlen(log_folder_path);
        if (path_len) {
            daemon_context->log_folder_path = static_cast<char *>(malloc(++path_len));
            memcpy(daemon_context->log_folder_path, log_folder_path, path_len);
        }
    }

    // creating interruption pipes.
    if (pipe(daemon_context->interrupter) < 0 ||
        !FDUtils::SetNonBlock(daemon_context->interrupter[0] || !FDUtils::SetNonBlock(daemon_context->interrupter[1]))) {
        StopDaemon();
        return false;
    }

    // creating a daemon thread.
    const int res = pthread_create(&daemon_context->daemon_thread, nullptr, DaemonFunction, nullptr);
    if (res) {
        return false;
    }
    return true;
}

bool HawkeyeDaemon::StopDaemon() {
    if (!daemon_context) {
        return false;
    }
    if (daemon_context->daemon_thread) {
        // writing to pipe in order to interrupt select.
        if (write(daemon_context->interrupter[1], (void *) "\0", 1) < 0) {
            return false;
        }
        pthread_join(daemon_context->daemon_thread, nullptr);
        close(daemon_context->interrupter[0]);
        close(daemon_context->interrupter[1]);
    }
    if (daemon_context->log_folder_path) {
        free(daemon_context->log_folder_path);
    }
    free(daemon_context);
    daemon_context = nullptr;
    return true;
}

void *HawkeyeDaemon::GetDaemonCallbacksArg() {
    if (!daemon_context) {
        return nullptr;
    }
    return daemon_context->callback_arg;
}

