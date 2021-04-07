//
// Created by #Suyghur, on 4/6/21.
//

#ifndef ZEGE_LOGGER_H
#define ZEGE_LOGGER_H

#include <string>
#include <android/log.h>

#define TAG "zege_sdk"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__) // 定义LOGD类型
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__) // 定义LOGD类型

//class logger {
//public:
//    static void logd(const std::string &msg);
//
//    static void loge(const std::string &msg);
//};


#endif //ZEGE_LOGGER_H
