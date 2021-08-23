//
// Created by #Suyghur, on 2021/08/19.
//

#include <dirent.h>
#include "hawkeye_thread_utils.h"

size_t ThreadUtils::GetThreads(pid_t pid, pid_t *out, size_t size) {
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
