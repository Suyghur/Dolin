//
// Created by #Suyghur, on 2021/08/19.
//

#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include "hawkeye_socket_utils.h"

void SocketUtils::FillSockAddr(const char *socket_name, struct sockaddr_un *out_address) {
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
