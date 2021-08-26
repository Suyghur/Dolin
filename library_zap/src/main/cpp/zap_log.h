//
// Created by #Suyghur, on 2021/08/25.
//

#ifndef DOLIN_ZAP_LOG_H
#define DOLIN_ZAP_LOG_H

#include <android/log.h>

#define TAG "dolin_zap"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__) // 定义LOGD类型
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__) // 定义LOGD类型

#endif //DOLIN_ZAP_LOG_H
