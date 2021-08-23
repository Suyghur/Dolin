//
// Created by #Suyghur, on 2021/08/23.
//

#ifndef DOLIN_HAWKEYE_FD_UTILS_H
#define DOLIN_HAWKEYE_FD_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

class FDUtils {
public:
    static bool SetNonBlock(int fd);
};

#ifdef __cplusplus
}
#endif


#endif //DOLIN_HAWKEYE_FD_UTILS_H
