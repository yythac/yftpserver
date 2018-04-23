# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := yftpserver-jni

LOCAL_C_INCLUDES := D:\softwares\lib-for-Android-master\boost

LOCAL_CPP_FEATURES := exceptions rtti

#VisualGDBAndroid: AutoUpdateSourcesInNextLine
#VisualGDBAndroid: AutoUpdateSourcesInNextLine
LOCAL_SRC_FILES := com_yythac_yftpserver_YFtpController.cpp YFtpServer.cpp yftp/client_manager.cpp yftp/client_node.cpp yftp/common.cpp yftp/connection.cpp yftp/conversion.cpp yftp/ftpserve_native.cpp yftp/ftp_server.cpp yftp/prebuild.cpp yftp/reply.cpp yftp/request_parser.cpp yftp/server.cpp yftp/user_manager.cpp yftp/user_node.cpp

LOCAL_LDLIBS := -llog
LOCAL_LDLIBS += -latomic

include $(BUILD_SHARED_LIBRARY)


