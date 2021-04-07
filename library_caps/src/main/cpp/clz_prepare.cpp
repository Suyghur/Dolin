//
// Created by #Suyghur, on 4/6/21.
//

#include <terminator.h>
#include <logger.h>
#include "libs/clz_prepare.h"


void JNICALL clzPrepare(jvmtiEnv *jvmti_env, JNIEnv *env, jthread thread, jclass clz) {
    if (isTerminated()) {
        LOGD("clzPrepare: terminated");
        return;
    }
    char *clz_signature = nullptr;
    jvmti_env->GetClassSignature(clz, &clz_signature, nullptr);

    char *stack_info=createStackInfo()
}
