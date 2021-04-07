//
// Created by #Suyghur, on 4/2/21.
//

#include <jni.h>
#include "libs/jvmti.h"
#include <string.h>
#include "libs/logger.h"
#include "clz_prepare.h"

static jvmtiEnv *localJvmtiEnv;


jvmtiEnv *createJvmtiEnv(JavaVM *vm) {
    jvmtiEnv *jvmti_env = nullptr;
    jint result = vm->GetEnv((void **) &jvmti_env, JVMTI_VERSION_1_2);
    if (result != JNI_OK) {
        return nullptr;
    }
    return jvmti_env;
}

void setAllCapabilities(jvmtiEnv *jvmti) {
    jvmtiCapabilities caps;
    jvmtiError error;
    error = jvmti->GetPotentialCapabilities(&caps);
    if (error != JVMTI_ERROR_NONE) {
        LOGE("error on GetPotentialCapabilities : %d", error);
    }
    error = jvmti->AddCapabilities(&caps);
    if (error != JVMTI_ERROR_NONE) {
        LOGE("error on AddCapabilities : %d", error);
    }
}


extern "C" JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options, void *reserved) {
    jvmtiEnv *jvmti_env = createJvmtiEnv(vm);

    if (jvmti_env == nullptr) {
        return JNI_ERR;
    }

    localJvmtiEnv = jvmti_env;
    setAllCapabilities(jvmti_env);

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
//    callbacks.ClassPrepare = &clzPrepare
    return JNI_OK;
}

static jlong initNative(JNIEnv *env, jobject thiz) {
    return 0;
}


static JNINativeMethod gMethods[] = {
        {"initNative", "()J", (void *) initNative}
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    jclass clz = env->FindClass("com/suyghur/zege/Zege");
    if (env->RegisterNatives(clz, gMethods, sizeof(gMethods) / sizeof(gMethods[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}