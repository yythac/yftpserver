									yftpserver 1.2版 使用说明
1、使用环境
理论上支持的操作系统：windows 7及以上操作系统，linux操作系统
测试过的操作系统：windows 2008 server，windows 10，centos 7.2 64（位）

2、文件说明
yftpserver-------------------------------------主执行程序
yftpserver.xml---------------------------------xml格式的配置文件	
yftpserver.properties--------------------------properties格式的配置文件
yftpserver.ini---------------------------------ini格式的配置文件
readme.txt-------------------------------------使用说明文件
server_certs-----------------------------------使用ssl连接时的证书文件

3、新增功能
3.1 使用ycommonserverlib网络库
3.2 支持显式和隐式FTPS协议

4、配置文件
	当3种配置文件都存在时，优先顺序为xml>properties>ini，详细信息参考配置文件中的注释
	1. 要使用普通的ftp服务器,修改配置文件中的 "server.use_ssl" 为 0
	2. 要使用隐式ftps服务器,修改配置文件中的 "server.use_ssl" 为 1
	3. 要使用显式ftps服务器,修改配置文件中的 "server.use_ssl" 和 "server.delay_ssl" 为 1	

5、安装及启动yftpserver
	
5.1 运行 “yftpserver -h" 显示帮助信息
	yftpserver -h
	usage: yftpserver OPTIONS
	--daemon        Run application as a daemon.
	--umask=mask    Set the daemon's umask (octal, e.g. 027).
	--pidfile=path  Write the process ID of the application to given file.
	-h, --help      display help information on command line arguments
	
5.2 直接运行“yftpserver” 使用普通方式运行ftp 服务器
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
	[2018-Apr-27 13:53:50.238557]<info>(0x0000108c): Ftp Server Root Dir:/
	[2018-Apr-27 13:53:50.239559]<info>(0x0000108c): Anonymous User Info:1:home:rwd
	[2018-Apr-27 13:53:50.272581]<info>(0x0000108c): Ftp Server User Infomation:tester1:123456::rwd
	[2018-Apr-27 13:53:50.277586]<info>(0x0000108c): Ftp Server pasv port range:10000-30000
	[2018-Apr-27 13:53:50.280587]<info>(0x0000108c): service pump started.
	
5.3 运行 “yftpserver --daemon" 以服务进程的方式启动ftp server
	yftpserver --daemon

6、个人信息
个人网站：http://www.yythac.com
github链接：https://github.com/yythac
email:yyt_hac@163.com