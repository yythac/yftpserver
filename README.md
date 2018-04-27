# yftpserver
    
    A cross platform ftp server using C++11 and Boost Library

    yftpserverwindows/yftp.sln----------------------------Microsoft Visual C++ 2015 Solution File for Windows,
                                Complile Successfully in Windows 7 with Microsoft Visual C++ 2015 and Boost Library l.55
    yftpserverlinux/yftpserverlinux.sln-------------------Microsoft Visual C++ 2015 + VisualGDB 5.0 Solution File for Linux,
                                Complile Successfully in CentOS(64-bit) with GCC 5.3 and Boost Library l.55
    yftpserverandroid/YFtpServerWapper.sln----------------Microsoft Visual C++ 2015 + VisualGDB 5.0 Solution File for Android,
                                Complile Successfully in Android 4.4.2 with NDK(r10d) and Boost Library l.55

    jni/yftp----------------------------------------------yftpserver core c++ source files
    src/com/yythac/yftpserver-----------------------------yftpserver java source files for android

	ver 1.1:
	1. use ycommonserverlib
	2. support xml,properties,ini format config file
	3. support Explicit or Implicit ftps
	
	How to config:
	1. to use as normal ftp server,modify "server.use_ssl" in config file to 0
	2. to use as Implicit ftps,modify "server.use_ssl" in config file to 1
	3. to use as Explicit ftps,modify "server.use_ssl"  and  "server.delay_ssl" to 1 in config file