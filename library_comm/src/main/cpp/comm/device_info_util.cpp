//
// Created by #Suyghur, on 2021/07/28.
//

#include "device_info_util.h"

std::string DeviceInfoUtil::GetDeviceManufacturer() {
    char *manufacturer = (char *) malloc(128 * sizeof(char));
    if (manufacturer == nullptr) {
        return "";
    }
    __system_property_get("ro.product.manufacturer", manufacturer);
    std::string result = manufacturer;
    free(manufacturer);
    return result;
}

std::string DeviceInfoUtil::GetDeviceModel() {
    char *model = (char *) malloc(128 * sizeof(char));
    if (model == nullptr) {
        return "";
    }
    __system_property_get("ro.product.model", model);
    std::string result = model;
    free(model);
    return result;
}

std::string DeviceInfoUtil::GetMobileBrand() {
    char *brand = (char *) malloc(128 * sizeof(char));
    if (brand == nullptr) {
        return "";
    }
    __system_property_get("ro.product.brand", brand);
    std::string result = brand;
    free(brand);
    return result;
}

std::string DeviceInfoUtil::GetDeviceSoftwareVersion() {
    char *version = (char *) malloc(128 * sizeof(char));
    if (version == nullptr) {
        return "";
    }
    __system_property_get("ro.build.version.release", version);
    std::string result = version;
    free(version);
    return result;
}

std::string DeviceInfoUtil::GetCpuAbi() {
    char *abi = (char *) malloc(128 * sizeof(char));
    if (abi == nullptr) {
        return "";
    }
    if (android_get_device_api_level() >= 21) {
        __system_property_get("ro.product.cpu.abilist", abi);
    } else {
        char *abi32 = (char *) malloc(128 * sizeof(char));
        char *abi64 = (char *) malloc(128 * sizeof(char));
        __system_property_get("ro.product.cpu.abilist32", abi32);
        __system_property_get("ro.product.cpu.abilist64", abi64);
        if (strcmp(abi64, "") == 0) {
            sprintf(abi, "%s", abi32);
        } else {
            sprintf(abi, "%s,%s", abi64, abi32);
        }
        free(abi32);
        free(abi64);
    }
    std::string result = abi;
    free(abi);
    return result;
}
