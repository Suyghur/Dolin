//
// Created by #Suyghur, on 4/6/21.
//

#include <logger.h>
#include "includes/zege_drive.h"


jvmtiEnv *getJvmtiEnv(JavaVM *vm) {
    return nullptr;
}

jvmtiEnv *getJvmtiEnvFromJNI(JNIEnv *env) {
    return nullptr;
}

JavaVM *getJavaVM(JNIEnv *env) {
    return nullptr;
}

/**
 * 获取指定对象的大小。对象的大小与JVM的具体实现相关，
 * 是该对象所占用存储空间的近似值，可能会包含某些或所有对象的开销，
 * 因此对象大小的比较，只在某个JVM实现内有意义，在不同JVM实现之间没有比较意思。
 * 对象的大小，在单次调用期间，也可能会发生变化。
 */
jlong getObjectSize(JNIEnv *env, jclass jclz, jobject jobj) {
    return 0;
}

char *createStackInfo(jvmtiEnv *jvmti_env, JNIEnv *env, jthread thread, int stack_depth) {
    char *result = nullptr;
    jvmtiFrameInfo frame_info[stack_depth];
    jint count;
    jvmtiError error;

    error = jvmti_env->GetStackTrace(thread, 0, stack_depth, frame_info, &count);
    if (error != JVMTI_ERROR_NONE) {
        LOGE("jvmti error on GetStackTrace: %i", error);
        return result;
    }

    if (count <= 0) {
        return result;
    }

    for (int i = 0; i < count; i++) {
        jvmtiFrameInfo info = frame_info[i];
        char *clz_signature = nullptr;
        char *method_name = nullptr;

        //获取方法名
        error = jvmti_env->GetMethodName(info.method, &method_name, nullptr, nullptr);
        if (error != JVMTI_ERROR_NONE) {
            LOGE("jvmti error on GetMethodName: %i", error);
            break;
        }

        //获取方法所在的类
        jclass declaring_clz;
        error = jvmti_env->GetMethodDeclaringClass(info.method, &declaring_clz);
        if (error != JVMTI_ERROR_NONE) {
            LOGE("jvmti error on GetMethodDeclaringClass: %i", error);
            break;
        }

        //获取方法所在类的签名
        error = jvmti_env->GetClassSignature(declaring_clz, &clz_signature, nullptr);
        if (error != JVMTI_ERROR_NONE) {
            LOGE("jvmti error on GetClassSignature: %i", error);
            break;
        }

        if (result == nullptr) {
            asprintf(&result, "%s%s%s", clz_signature, "^^^", method_name);
        } else {
            char *stack = nullptr;
            asprintf(&stack, "%s%s%s%s%s", result, ",,,", clz_signature, "^^^", method_name);
            free(result);
            result = stack;
        }

        jvmti_env->Deallocate(reinterpret_cast<unsigned char *>(clz_signature));
        jvmti_env->Deallocate(reinterpret_cast<unsigned char *>(method_name));
    }

    return result;
}


