//
// Created by #Suyghur, on 2021/08/18.
//

#ifndef DOLIN_HAWKEYE_DAEMON_H
#define DOLIN_HAWKEYE_DAEMON_H

#include <linux/un.h>
#include "hawkeye_private.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*hawkeye_daemon_crash_callback)(const char *log_path, void *args);

typedef void (*hawkeye_daemon_start_stop_callback)(void *args);


struct hawkeye_daemon_context {
    /// Pointer to unwinder initialization func
    unwinder_init_func_ptr unwinder_init;

    unwinder_release_func_ptr unwinder_release;

    unwinder_func_ptr unwinder_func;

    char *log_folder_path;

    char *log_file;

    int interrupter[2];

    pthread_t daemon_thread;

    hawkeye_daemon_start_stop_callback start_callback;
    hawkeye_daemon_start_stop_callback stop_callback;
    hawkeye_daemon_crash_callback crash_callback;

    void *callback_arg;

    struct sockaddr_un socket_address;
};

class HawkeyeDaemon {
public:

    static bool PtraceAttach(pid_t tid);

    static void PtraceDetach(pid_t tid);

    static bool DaemonCreateReport(struct hawkeye_crash_message *message);

    static void DaemonProcessClient(int client_socket);

    static void *DaemonFunction(void *args);

    static bool StartDaemon(const char *socket_name, const char *log_folder_path, hawkeye_daemon_start_stop_callback start_callback,
                            hawkeye_daemon_crash_callback crash_callback, hawkeye_daemon_start_stop_callback stop_callback,
                            void *callback_arg);

    static bool StopDaemon();

    static void *GetDaemonCallbacksArg();

private:
    static const int SOCKET_BACKLOG = 1;

};

#ifdef __cplusplus
}
#endif


#endif //DOLIN_HAWKEYE_DAEMON_H
