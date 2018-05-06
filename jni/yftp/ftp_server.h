/*
* ftp_server.h
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#pragma once

#define BOOST_ALL_NO_LIB

#include <ctime>
#include <cctype>
#include <cstdio>
//#include <fstream>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <stdarg.h>
#include <mutex>
//#include <thread>
//#include <atomic>


//#include <sys/stat.h>
#include <boost/asio.hpp>
#ifdef SERVER_APP
#include <boost/asio/ssl.hpp>
#endif
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <boost/filesystem.hpp>

using namespace boost::filesystem;


#include "request_parser.h"
#include "user_manager.h"

#define LOG_TAG "yftpserver"
#ifndef WIN32

//#include <../../common/c/JniLog.h>

#else

#include "../common/c/JniLog.h"

#endif


namespace ftp {
	namespace server {

		/***************************************
		 * THE CLASS: ftp_server
		 ***************************************/
		class ftp_server
		{

		public:

			/* Constructor */
			ftp_server(void);
			/* Destructor */
			~ftp_server(void);
#ifdef SERVER_APP

			static void ssl_context(void* c)
			{
				context_ = (boost::asio::ssl::context*)c;
			}
			static boost::asio::ssl::context* ssl_context()
			{
				return context_;
			}

#endif
			//设置pasv模式数据端口范围
			static bool set_data_port_range(unsigned short int data_port_start, unsigned int number)
			{
				if (data_port_start <= 1024 || number == 0)
				{
					return false;
				}
				data_port_range_.start = data_port_start;
				data_port_range_.num = number;

				return true;
			}
			//获取pasv模式数据端口范围
			static bool get_data_port_range(unsigned short int *data_port_start, int *number)
			{

				if (data_port_start && number)
				{

					*data_port_start = data_port_range_.start;
					*number = data_port_range_.num;
					return true;

				}
				return false;

			}

			//获取错误码
			int get_last_error()
			{
				return last_error_.get_error_code();
			}
			//获取错误字符串
			wstring get_last_error_string()
			{
				return last_error_.to_string();
			}

			//添加用户
			//参数：
			//name：需要添加的用户名
			//password:用户密码
			//home_dir:用户起始目录
			//priv:用户权限
			//返回：	
			//true:添加用户成功
			//false:添加用户失败
			bool add_user(const wstring& name, const wstring& password
				, const wstring& home_dir, unsigned char priv);
			bool delete_user(const wstring& name);
			//设置用户权限
			bool set_user_privilege(const std::wstring& name, unsigned char priv)
			{
				return user_manager_.set_user_privilege(name, priv);
			}
			//设置用户密码
			bool set_user_password(const std::wstring& name, const std::wstring& password)
			{
				return user_manager_.set_user_password(name, password);
			}
			//设置用户目录
			bool set_user_home_directory(const std::wstring& name, const std::wstring& home_dir)
			{
				return user_manager_.set_user_home_directory(name, home_dir);
			}
			//初始化匿名用户
			bool allow_anonymous(bool do_allow, const std::wstring& home_dir, unsigned char priv=FTP_USER_PRIV_READ);
			//判断是否允许匿名登录
			bool is_anonymous_allowed(void) 
			{

				return this->is_allow_anonymous_;

			}
			//连接开始
			bool start_work(reply& ftpreply);

			//处理ftp命令
			bool process_command(YCOMMON::YSERVER::i_ycommon_socket& ctrl_socket, char *pdata, int datalen, reply& ftpreply);
			//连接结束
			bool end_work(YCOMMON::YSERVER::i_ycommon_socket& ctrl_socket);

			/***************************************
			 * PRIVATE
			 ***************************************/
		private:

			static data_port_range data_port_range_;
#ifdef SERVER_APP
			static boost::asio::ssl::context* context_;

#endif

			user_manager user_manager_;

			//错误处理
			error_manager last_error_;
			/*****************************************
			 * CONFIG
			 *****************************************/

			bool is_allow_anonymous_;



		};
	}
}

