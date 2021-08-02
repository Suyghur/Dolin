//
// Created by #Suyghur, on 2021/07/28.
//

#ifndef DOLIN_COMM_KITS_H
#define DOLIN_COMM_KITS_H


#include <jni.h>

class StringUtil {
public:
    static jstring GetJString(JNIEnv *env, const char *src);
};

#endif //DOLIN_COMM_KITS_H
