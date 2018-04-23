APP_ABI := armeabi
LOCAL_ARM_MODE  := arm
APP_PLATFORM := android-8

#APP_STL := stlport_static  
#NDK_TOOLCHAIN_VERSION=4.8 #ʹ��GCC4.7  
APP_STL := c++_static#GNU STL  
APP_CPPFLAGS = -frtti -fexceptions -std=c++11 -fpermissive
NDK_TOOLCHAIN_VERSION := clang
