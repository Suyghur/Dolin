//
// Created by #Suyghur, on 2021/07/28.
//

#include "string_util.h"
#include "string"

jstring StringUtil::GetJString(JNIEnv *env, const char *src) {
    jclass clz = env->FindClass("java/lang/String");
    jmethodID methodID = env->GetMethodID(clz, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("utf-8");

    auto length = static_cast<jsize>(strlen(src));
    jbyteArray bytes = env->NewByteArray(length);
    env->SetByteArrayRegion(bytes, 0, length, (jbyte *) src);
    auto result = (jstring) env->NewObject(clz, methodID, bytes, encoding);

    env->DeleteLocalRef(clz);
    env->DeleteLocalRef(encoding);
    env->DeleteLocalRef(bytes);

    return result;
}
