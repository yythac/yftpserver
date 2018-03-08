#ifndef WIN32
#include <android/log.h>

#ifndef _Included_JniLog_H
#define _Included_JniLog_H
#ifdef __cplusplus
extern "C" {
#endif

    #ifndef LOG_TAG
    #define LOG_TAG NULL
    #endif

    #define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
    #define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG ,  LOG_TAG, __VA_ARGS__)
    #define LOGI(...) __android_log_print(ANDROID_LOG_INFO  ,  LOG_TAG, __VA_ARGS__)
    #define LOGW(...) __android_log_print(ANDROID_LOG_WARN  ,  LOG_TAG, __VA_ARGS__)
    #define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif
#else

#define LOGV(...) ;
#define LOGD(...) ;
#define LOGI(...) ;
#define LOGW(...) ;
#define LOGE(...) ;

#endif