//
// Created by #Suyghur, on 2021/08/23.
//

#include "hawkeye_fd_utils.h"
#include "hawkeye_log.h"
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>


bool FDUtils::SetNonBlock(int fd) {
    int socket_flags = fcntl(fd, F_GETFL);
    if (socket_flags == -1) {
        LOGE("Couldn't get fcntl flags, error: %s (%d)", strerror(errno), errno);
        return false;
    }
    if (socket_flags & O_NONBLOCK) {
        return true;
    }
    socket_flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, socket_flags) == -1) {
        LOGE("Couldn't set fcntl flags, error: %s (%d)", strerror(errno), errno);
        return false;
    }
    return true;
}
