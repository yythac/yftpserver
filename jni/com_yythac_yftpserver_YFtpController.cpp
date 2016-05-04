#include <string.h>
#include <jni.h>
//#include <stdlib.h>

#define LOG_TAG "yftpserver"
#include <../../common/c/JniLog.h>

#include "yftp/ftpserver_native.h"
#include "com_yythac_yftpserver_YFtpController.h"



std::wstring JavaToWSZ(JNIEnv* env, jstring string)
{
    std::wstring value;
    if (string == NULL) {
        return value; // empty string
    }
    const jchar* raw = env->GetStringChars(string, NULL);
    if (raw != NULL) {
        jsize len = env->GetStringLength(string);
        value.assign(raw, raw+len);
        env->ReleaseStringChars(string, raw);
    }
    return value;
}

#if 1
/* This is a trivial JNI example where we use a native method
 * to return a new VM String. See the corresponding Java source
 * file located at:
 *
 *   apps/samples/hello-jni/project/src/com/example/hellojni/HelloJni.java
 */
JNIEXPORT jstring JNICALL Java_com_yythac_yftpserver_YFtpController_stringFromJNI( JNIEnv* env,
                                                  jobject thiz )
{
#if defined(__arm__)
  #if defined(__ARM_ARCH_7A__)
    #if defined(__ARM_NEON__)
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a/NEON (hard-float)"
      #else
        #define ABI "armeabi-v7a/NEON"
      #endif
    #else
      #if defined(__ARM_PCS_VFP)
        #define ABI "armeabi-v7a (hard-float)"
      #else
        #define ABI "armeabi-v7a"
      #endif
    #endif
  #else
   #define ABI "armeabi"
  #endif
#elif defined(__i386__)
   #define ABI "x86"
#elif defined(__x86_64__)
   #define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
   #define ABI "mips64"
#elif defined(__mips__)
   #define ABI "mips"
#elif defined(__aarch64__)
   #define ABI "arm64-v8a"
#else
   #define ABI "unknown"
#endif

    return env->NewStringUTF( "Hello from JNI !  Compiled with ABI " ABI ".");
}
#endif


/*
 * Class:     com_yythac_yftpserver_YFtpController
 * Method:    StartFtpServer
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_yythac_yftpserver_YFtpController_StartFtpServer
  (JNIEnv *env, jobject thiz)
{
	LOGI("Entering Java_com_yythac_yftpserver_YFtpController_StartFtpServer2....");

	//init_ftp_server();


	return start_ftp_server();
}

/*
 * Class:     com_yythac_yftpserver_YFtpController
 * Method:    StopFtpServer
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_yythac_yftpserver_YFtpController_StopFtpServer
  (JNIEnv *env, jobject thiz)
{
	LOGI("Entering Java_com_yythac_yftpserver_YFtpController_StopFtpServer....");

	return stop_ftp_server();
}

/*
 * Class:     com_yythac_yftpserver_YFtpController
 * Method:    SetListenPort
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_com_yythac_yftpserver_YFtpController_InitFtpServer
  (JNIEnv *env, jobject thiz, jboolean bDoAllow,jstring szusername,jstring szpassword,jstring szStartPath,jbyte ucPriv,jint port)
{
	LOGI("Entering Java_com_yythac_yftpserver_YFtpController_InitFtpServer....");
	if(port <= 0)
	{
		return false;
	}

	return init_ftp_server(bDoAllow,(const wchar_t *)JavaToWSZ(env,szusername).c_str()
			,(const wchar_t *)JavaToWSZ(env,szpassword).c_str()
			,(const wchar_t *)JavaToWSZ(env,szStartPath).c_str(),ucPriv,port);
}


