#include <jni.h>
#include <string>

extern "C" {
#include "libavcodec/avcodec.h"
JNIEXPORT jstring JNICALL
Java_com_eli_helloffmpeg_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
//    std::string hello = "Hello   from  C++";
//    return env->NewStringUTF(hello.c_str());

    char info[10000] = {0};
    sprintf(info, "%s\n", avcodec_configuration());
//    return env->NewString((const jchar *) info, 10000);
    return env->NewStringUTF(info);
}
}
