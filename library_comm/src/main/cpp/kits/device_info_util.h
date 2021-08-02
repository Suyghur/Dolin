//
// Created by #Suyghur, on 2021/07/28.
//

#ifndef DOLIN_DEVICE_INFO_UTIL_H
#define DOLIN_DEVICE_INFO_UTIL_H


#include <string>
#include <jni.h>

class DeviceInfoUtil {
public:
    static std::string GetDeviceManufacturer();

    static std::string GetDeviceModel();

    static std::string GetMobileBrand();

    static std::string GetDeviceSoftwareVersion();

    static std::string GetCpuAbi();
};


#endif //DOLIN_DEVICE_INFO_UTIL_H
