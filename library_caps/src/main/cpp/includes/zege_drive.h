//
// Created by #Suyghur, on 4/6/21.
//

#ifndef ZEGE_ZEGE_DRIVE_H
#define ZEGE_ZEGE_DRIVE_H


#include <jni.h>
#include "jvmti.h"

jvmtiEnv *getJvmtiEnv(JavaVM *vm);

jvmtiEnv *getJvmtiEnvFromJNI(JNIEnv *env);

JavaVM *getJavaVM(JNIEnv *env);

jlong getObjectSize(JNIEnv *env, jclass jclz, jobject jobj);

char *createStackInfo(jvmtiEnv *jvmti_env, JNIEnv *env, jthread thread, int stack_depth);

#endif //ZEGE_ZEGE_DRIVE_H
