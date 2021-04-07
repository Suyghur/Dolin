//
// Created by #Suyghur, on 4/6/21.
//

#ifndef ZEGE_CLZ_PREPARE_H
#define ZEGE_CLZ_PREPARE_H

#include <jni.h>
#include "jvmti.h"

void JNICALL clzPrepare(jvmtiEnv *jvmti_env, JNIEnv *env, jthread thread, jclass clz);

#endif //ZEGE_CLZ_PREPARE_H
