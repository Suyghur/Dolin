//
// Created by #Suyghur, on 4/13/21.
//

#include <jni.h>
#include <device_info_util.h>
#include <string_util.h>

constexpr size_t prime = 0x100000001B3ull;
constexpr size_t basis = 0xCBF29CE484222325ull;

size_t hash_run_time(const char *str) {
    size_t ret = basis;
    while (*str) {
        ret ^= *str;
        ret *= prime;
        str++;
    }

    return ret;
}

constexpr size_t hash_compile_time(const char *str, size_t value = basis) {
    return *str ? hash_compile_time(str + 1, (*str ^ value) * prime) : value;
}

constexpr size_t operator ""_hash(const char *p, size_t) {
    return hash_compile_time(p);
}

static jstring NativeInvoke(JNIEnv *env, jclass clazz, jstring method) {
    const char *method_ = env->GetStringUTFChars(method, JNI_FALSE);
    std::string result;
    switch (hash_run_time(method_)) {
        case "GetDeviceManufacturer"_hash:
            result = DeviceInfoUtil::GetDeviceManufacturer();
            break;
        case "GetDeviceModel"_hash:
            result = DeviceInfoUtil::GetDeviceModel();
            break;
        case "GetMobileBrand"_hash:
            result = DeviceInfoUtil::GetMobileBrand();
            break;
        case "GetDeviceSoftwareVersion"_hash:
            result = DeviceInfoUtil::GetDeviceSoftwareVersion();
            break;
        case "GetCpuAbi"_hash:
            result = DeviceInfoUtil::GetCpuAbi();
            break;
        default:
            result = "";
            break;
    }
    jstring jresult = StringUtil::GetJString(env, result.c_str());
    env->ReleaseStringUTFChars(method, method_);
    return jresult;
}

static JNINativeMethod gMethod[] = {
        {"nativeInvoke", "(Ljava/lang/String;)Ljava/lang/String;", (void *) NativeInvoke}
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clz = env->FindClass("com/dolin/comm/impl/NativeBridge");
    if (env->RegisterNatives(clz, gMethod, sizeof(gMethod) / sizeof(gMethod[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}
