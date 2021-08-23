//
// Created by #Suyghur, on 2021/08/19.
//

#ifndef DOLIN_HAWKEYE_SOCKET_UTILS_H
#define DOLIN_HAWKEYE_SOCKET_UTILS_H

#include <linux/un.h>

#ifdef __cplusplus
extern "C" {
#endif

class SocketUtils {
public:
    static void FillSockAddr(const char *socket_name, struct sockaddr_un *out_address);
};

#ifdef __cplusplus
}
#endif

#endif //DOLIN_HAWKEYE_SOCKET_UTILS_H
