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

	ver 1.2:
	1. use ycommonserverlib
	2. support xml,properties,ini format config file
	3. support Explicit or Implicit ftps
	
	How to config:
	1. to use as normal ftp server,modify "server.use_ssl" in config file to 0
	2. to use as Implicit ftps,modify "server.use_ssl" in config file to 1
	3. to use as Explicit ftps,modify "server.use_ssl"  and  "server.delay_ssl" to 1 in config file
	
	usage:
	
	1. run yftpserver with param '/h' to show help information
	yftpserver /h
	usage: yftpserver OPTIONS
	/registerService           Register the application as a service.
	/unregisterService         Unregister the application as a service.
	/displayName=name          Specify a display name for the service (only with
							   /registerService).
	/description=text          Specify a description for the service (only with
							   /registerService).
	/startup=automatic|manual  Specify the startup mode for the service (only with
							   /registerService).
	/help                      display help information on command line arguments
	
	2. run yftpserver with no param to start ftp server
	yftpserver
	
	[2018-Apr-27 13:53:49.963378]<info>(0x0000108c): log_level:0,log_to_console:1,log_to_file:0
	[2018-Apr-27 13:53:49.967365]<info>(0x0000108c): usessl:1,userawdata:1,delayssl:1
	[2018-Apr-27 13:53:50.218543]<info>(0x0000108c): server.certificate_chain_file:server_certs/server.pem
	[2018-Apr-27 13:53:50.220544]<info>(0x0000108c): server.private_key_file:server_certs/server.pem
	[2018-Apr-27 13:53:50.222545]<info>(0x0000108c): server.key_file_format:pem
	[2018-Apr-27 13:53:50.224550]<info>(0x0000108c): server.tmp_dh_file:server_certs/dh2048.pem
	[2018-Apr-27 13:53:50.227550]<info>(0x0000108c): server bind ip:0.0.0.0,server port:21,io_thread_num:-1
	[2018-Apr-27 13:53:50.230552]<info>(0x0000108c): pool_min_thread_num:5,pool_max_thread_num:32
	[2018-Apr-27 13:53:50.236574]<info>(0x0000108c): Begin to init Ftp Server.......
	[2018-Apr-27 13:53:50.238557]<info>(0x0000108c): Ftp Server Root Dir:c:\
	[2018-Apr-27 13:53:50.239559]<info>(0x0000108c): Anonymous User Info:1:users\public:rwd
	[2018-Apr-27 13:53:50.272581]<info>(0x0000108c): Ftp Server User Infomation:tester1:123456::rwd
	[2018-Apr-27 13:53:50.277586]<info>(0x0000108c): Ftp Server pasv port range:10000-30000
	[2018-Apr-27 13:53:50.280587]<info>(0x0000108c): service pump started.
	
	3. run yftpserver with param "/registerService" to Register the application as a service
	yftpserver /registerService
	