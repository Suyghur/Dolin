//
// Created by #Suyghur, on 2021/08/30.
//

#include "hawkeye_private.h"

#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <dirent.h>

void FillSockAddr(const char *socket_name, struct sockaddr_un *out_address) {
    size_t socket_name_len = strlen(socket_name);
    // discarding exceeding characters.
    if (socket_name_len > UNIX_PATH_MAX - 1) {
        socket_name_len = UNIX_PATH_MAX - 1;
    }
    memset(out_address, 0, sizeof(struct sockaddr_un));
    out_address->sun_family = PF_UNIX;
    // the socket is abstract, the first byte should be 0, see "man 7 unix" for details.
    out_address->sun_path[0] = '\0';
    memcpy(out_address->sun_path + 1, socket_name, socket_name_len);
}

size_t GetThreads(pid_t pid, pid_t *out, size_t size) {
    // should have sufficient space to save "/proc/2147483647/task" including \0
    char path[22];
    snprintf(path, sizeof(path), "/proc/%d/task", (int) pid);

    // opening a directory for iteration.
    DIR *dir = opendir(path);
    if (!dir) {
        return 0;
    }

    pid_t *it = out;
    struct dirent *entry;
    while (it - out < size && (entry = readdir(dir))) {
        const auto tid = (pid_t) atoi(entry->d_name);
        if (!tid || tid == pid) {
            continue;
        }
        *it = tid;
        ++it;
    }

    // close
    closedir(dir);
    return it - out;
}
