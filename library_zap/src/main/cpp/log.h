//
// Created by #Suyghur, on 2021/08/10.
//

#ifndef DOLIN_HAWKEYE_LOG_H
#define DOLIN_LOG_H

#include <android/log.h>

#define TAG "dolin_jni"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__) // 定义LOGD类型
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__) // 定义LOGD类型

#endif //DOLIN_HAWKEYE_LOG_H
