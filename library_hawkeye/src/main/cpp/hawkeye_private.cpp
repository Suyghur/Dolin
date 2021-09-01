//
// Created by #Suyghur, on 2021/08/30.
//

#include "hawkeye_private.h"
#include "hawkeye_log.h"
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <dirent.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/utsname.h>
#include <cctype>
#include <ctime>

int Atoi(const char *str, int *i) {
    // We have to do this job very carefully for some unusual version of stdlib.
    long val = 0;
    char *end_ptr = nullptr;
    const char *p = str;

    // check.
    if (str == nullptr || i == nullptr) {
        return -1;
    }
    if ((*p < '0' || *p > '9') && *p != '-') {
        return -1;
    }
    p++;
    while (*p) {
        if (*p < '0' || *p > '9') {
            return -1;
        }
        p++;
    }

    // convert.
    errno = 0;
    val = strtol(str, &end_ptr, 10);

    // check.
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
        return -1;
    if (end_ptr == str)
        return -1;
    if (val > INT_MAX || val < INT_MIN)
        return -1;

    // ok.
    *i = (int) val;
    return 0;
}

char *Trim(char *start) {
    char *end = nullptr;
    if (start == nullptr) {
        return nullptr;
    }
    end = start + strlen(start);
    if (start == end) {
        return start;
    }
    while (start < end && isspace((int) (*start))) {
        start++;
    }
    if (start == end) {
        return start;
    }
    while (start < end && isspace((int) (*(end - 1)))) {
        end--;
    }
    *end = '\0';
    return start;
}

void GetSubSection(MmapGuard *mmap_ptr, const char *path, const char *title, size_t limit) {
    FILE *fp = nullptr;
    char line[512];
    char *p = nullptr;
    size_t n = 0;

    if ((fp = fopen(path, "r")) == nullptr) goto end;
    RecordNewline(mmap_ptr, "%s", title);
    while (fgets(line, sizeof(line), fp) != nullptr) {
        p = Trim(line);
        if (strlen(p) > 0) {
            n++;
            if (limit == 0 || n <= limit) {
                RecordNewline(mmap_ptr, "  %s", p);
            }
        }
    }
    if (limit > 0 && n > limit) {
        RecordNewline(mmap_ptr, "  ......");
        RecordNewline(mmap_ptr, "  (number of records: %zu)", n);
    }
    RecordNewline(mmap_ptr, "-");
    end:
    if (fp != nullptr) {
        fclose(fp);
    }
}


void Stamp2Standard(long ts, char *date, size_t len) {
    time_t tick = ts;
    struct tm tm{};
    tm = *localtime(&tick);
    strftime(date, len, "%Y-%m-%d %H:%M:%S", &tm);
}

void RecordNewline(MmapGuard *mmap_ptr, const char *format, ...) {
    // writing file to log may be disabled.
    if (mmap_ptr == nullptr) {
        return;
    }

    char *buffer = static_cast<char *>(malloc(HAWKEYE_LOG_BUFFER_SIZE));
//   {
//        va_list args;
//        va_start(args, format);
//        __android_log_vprint(ANDROID_LOG_ERROR, TAG, format, args);
//        va_end(args);
//    }

    // writing to a buffer.
    int printed;
    {
        va_list args;
        va_start(args, format);
        printed = vsnprintf(buffer, HAWKEYE_LOG_BUFFER_SIZE, format, args);
        va_end(args);
    }

    // printed contains the number of characters that would have been written if n had been sufficiently
    // large, not counting the terminating null character.
    if (printed > 0) {
        if (printed >= HAWKEYE_LOG_BUFFER_SIZE) {
            printed = HAWKEYE_LOG_BUFFER_SIZE - 1;
        }

        // replacing last buffer character with new line.
        buffer[printed] = '\n';
        // writing to a file including \n character.
        mmap_ptr->Write(buffer, (size_t) printed + 1);

    }
    free(buffer);
}

void RecordLine(MmapGuard *mmap_ptr, const char *format, ...) {
    // writing file to log may be disabled.
    if (mmap_ptr == nullptr) {
        return;
    }

    char *buffer = static_cast<char *>(malloc(HAWKEYE_LOG_BUFFER_SIZE));

    // writing to a buffer.
    int printed;
    {
        va_list args;
        va_start(args, format);
        printed = vsnprintf(buffer, HAWKEYE_LOG_BUFFER_SIZE, format, args);
        va_end(args);
    }

    // printed contains the number of characters that would have been written if n had been sufficiently
    // large, not counting the terminating null character.
    if (printed > 0) {
        if (printed >= HAWKEYE_LOG_BUFFER_SIZE) {
            printed = HAWKEYE_LOG_BUFFER_SIZE - 1;
        }
        // writing to a file not included \n character.
        mmap_ptr->Write(buffer, (size_t) printed + 1);
    }
    free(buffer);
}

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

void GetABIs(char *buf, size_t len) {
    if (buf == nullptr) {
        return;
    }
    if (android_get_device_api_level() >= 21) {
        __system_property_get("ro.product.cpu.abilist", buf);
    } else {
        char *abi32 = static_cast<char *>(malloc(64 * sizeof(char)));
        char *abi64 = static_cast<char *>(malloc(64 * sizeof(char)));
        __system_property_get("ro.product.cpu.abilist32", abi32);
        __system_property_get("ro.product.cpu.abilist64", abi64);
        if (strcmp(abi64, "") == 0) {
            snprintf(buf, len, "%s", abi32);
        } else {
            snprintf(buf, len, "%s,%s", abi64, abi32);
        }
        free(abi32);
        free(abi64);
    }
}


void GetKernelVersion(char *buf, size_t len) {
    struct utsname uts{};
    if (uname(&uts) != 0) {
        strncpy(buf, "unknown", len);
        buf[len - 1] = '\0';
        return;
    }
    snprintf(buf, len, "%s version %s %s (%s)", uts.sysname, uts.release, uts.version, uts.machine);
}

void GetLogcatInfo(MmapGuard *mmap_ptr, pid_t pid, const char *stack_name, size_t lines, char priority) {
    // system lines: 50
    // native logcat events lines: 50
    // native logcat main lines: 200
    FILE *fp;
    char cmd[128];
    char buf[1025];
    char pid_filter[64] = "";
    char pid_label[32] = "";

    // Since Android 7.0 Nougat (API level 24), logcat has --pid filter option.
    if (android_get_device_api_level() >= 24) {
        // api level >= 24, filtered by --pid option.
        snprintf(pid_filter, sizeof(pid_filter), "--pid %d ", pid);
    } else {
        // api level < 24, filtered by ourself, so we need to read more lines.
        snprintf(pid_label, sizeof(pid_label), " %d ", pid);
    }

    snprintf(cmd, sizeof(cmd), "/system/bin/logcat -b %s -d -v threadtime -t %u %s*:%c", stack_name, lines, pid_filter, priority);
    RecordNewline(mmap_ptr, "--------- tail end of log %s (%s)", stack_name, cmd);
    if ((fp = popen(cmd, "r")) != nullptr) {
        buf[sizeof(buf) - 1] = '\0';
        while (nullptr != fgets(buf, 1024, fp)) {
            if (android_get_device_api_level() >= 24 || nullptr != strstr(buf, pid_label)) {
                RecordLine(mmap_ptr, buf);
            }
        }
        pclose(fp);
    }
}

void GetFds(MmapGuard *mmap_ptr, pid_t pid) {
    int fd = -1;
    char path[128];
    char fd_path[512];
    char buf[512];
    long n, i;
    int fd_num;
    size_t total = 0;
    hawkeye_dirent_t *ent;
    ssize_t len;

    snprintf(path, sizeof(path), "/proc/%d/fd", pid);

    if ((fd = HAWKEYE_TEMP_FAILURE_RETRY(open(path, O_RDONLY | O_DIRECTORY | O_CLOEXEC))) < 0) goto end;

    while ((n = syscall(HAWKEYE_SYSCALL_GETDENTS, fd, buf, sizeof(buf))) > 0) {
        for (i = 0; i < n;) {
            ent = (hawkeye_dirent_t *) (buf + i);

            // get thd fd.
            if ('\0' == ent->d_name[0]) goto next;
            if (0 == memcmp(ent->d_name, ".", 1)) goto next;
            if (0 == memcmp(ent->d_name, "..", 2)) goto next;
            if (Atoi(ent->d_name, &fd_num) != 0) goto next;
            if (fd_num < 0) goto next;

            // count.
            total++;
            if (total > 1024) {
                goto next;
            }

            // read link of path.
            snprintf(path, sizeof(path), "/proc/%d/fd/%d", pid, fd_num);
            len = readlink(path, fd_path, sizeof(fd_path) - 1);
            if (len <= 0 || len > (ssize_t) (sizeof(fd_path) - 1)) {
                strncpy(fd_path, "???", sizeof(fd_path));
            } else {
                fd_path[len] = '\0';
            }

            // dump.
            RecordNewline(mmap_ptr, "    fd %d: %s", fd_num, fd_path);

            next:
            i += ent->d_reclen;
        }
    }
    end:
    if (total > 1024) {
        RecordNewline(mmap_ptr, "    ......");
    }
    RecordNewline(mmap_ptr, "    (number of FDs: %zu)", total);
    if (fd > 0) {
        close(fd);
    }
}

void GetNetworkInfo(MmapGuard *mmap_ptr, pid_t pid) {
    char path[128];
    if (android_get_device_api_level() >= 29) {
        RecordNewline(mmap_ptr, "Not supported on Android Q (API level 29) and later.");
    } else {
        snprintf(path, sizeof(path), "/proc/%d/net/tcp", pid);
        GetSubSection(mmap_ptr, path, " TCP over IPV4 (From: /proc/PID/net/tcp)", 1024);

        snprintf(path, sizeof(path), "/proc/%d/net/tcp6", pid);
        GetSubSection(mmap_ptr, path, " TCP over IPv6 (From: /proc/PID/net/tcp6)", 1024);

        snprintf(path, sizeof(path), "/proc/%d/net/udp", pid);
        GetSubSection(mmap_ptr, path, " UDP over IPv4 (From: /proc/PID/net/udp)", 1024);

        snprintf(path, sizeof(path), "/proc/%d/net/udp6", pid);
        GetSubSection(mmap_ptr, path, " UDP over IPv6 (From: /proc/PID/net/udp6)\n", 1024);

        snprintf(path, sizeof(path), "/proc/%d/net/icmp", pid);
        GetSubSection(mmap_ptr, path, " ICMP in IPv4 (From: /proc/PID/net/icmp)\n", 256);

        snprintf(path, sizeof(path), "/proc/%d/net/icmp6", pid);
        GetSubSection(mmap_ptr, path, " ICMP in IPv6 (From: /proc/PID/net/icmp6)\n", 256);

        snprintf(path, sizeof(path), "/proc/%d/net/unix", pid);
        GetSubSection(mmap_ptr, path, " UNIX domain (From: /proc/PID/net/unix)\n", 256);
    }
}
