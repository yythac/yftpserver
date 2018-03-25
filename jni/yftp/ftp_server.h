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
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
//#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

#include <boost/filesystem.hpp>

using namespace boost::filesystem;


#include "request_parser.hpp"
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

			static data_port_range data_port_range_;

			//获取错误码
			int get_last_error()
			{
				return last_error_.get_error_code();
			}
			wstring get_last_error_string()
			{
				return last_error_.to_string();
			}
			/* Set the TCP Port Range ftp_server can use to Send and Receive Files or Data.
			Arguments:
			-the First Port of the Range.
			-the Number of Ports, including the First previously given.
			Returns:
			-on success: true.
			-on error: false.
			*/
			bool set_data_port_range(unsigned short int data_port_start, unsigned int number)
			{
				if (data_port_start <= 1024 || number == 0)
				{
					return false;
				}
				this->data_port_range_.start = data_port_start;
				this->data_port_range_.num = number;

				return true;
			}

			/* Get the TCP Port Range ftp_server can use to Send and Receive Files or Data.
			Arguments:
			-a Pointer to the First Port.
			-a Pointer to the Number of Ports, including the First.
			Returns:
			-on success: true.
			-on error: false.
			*/
			bool get_data_port_range(unsigned short int *data_port_start, int *number) 
			{

				if (data_port_start && number) 
				{

					*data_port_start = this->data_port_range_.start;
					*number = this->data_port_range_.num;
					return true;

				}
				return false;

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
			bool set_user_privilege(const std::wstring& name, unsigned char priv)
			{
				return user_manager_.set_user_privilege(name, priv);
			}
			bool set_user_password(const std::wstring& name, const std::wstring& password)
			{
				return user_manager_.set_user_password(name, password);
			}
			bool set_user_home_directory(const std::wstring& name, const std::wstring& home_dir)
			{
				return user_manager_.set_user_home_directory(name, home_dir);
			}
			/* Allow or disallow Anonymous users. Its privilegies will be set to
			ftp_server::READFILE | ftp_server::LIST.
			Arguments:
			-true if you want ftp_server to accept anonymous clients, otherwise false.
			-the Anonymous User Start Directory.
			Returns:
			-on success: true.
			-on error: false.
			*/
			bool allow_anonymous(bool do_allow, const std::wstring& home_dir, unsigned char priv=FTP_USER_PRIV_READ);

			/* Check if Anonymous Users are allowed.
			Returns:
			-true: if Anonymous Users are allowed.
			-false: if Anonymous Users aren't allowed.
			*/
			bool is_anonymous_allowed(void) 
			{

				return this->is_allow_anonymous_;

			}


			bool process_command(boost::asio::ip::tcp::socket& ctrl_socket, char *pdata, int datalen, reply& ftpreply);
			bool start_work(reply& ftpreply);
			bool end_work(boost::asio::ip::tcp::socket& ctrl_socket);
			/***************************************
			 * PRIVATE
			 ***************************************/
		private:

			user_manager user_manager_;
			//request_parser request_parser_;
			//错误处理
			error_manager last_error_;
			/*****************************************
			 * CONFIG
			 *****************************************/

			bool is_allow_anonymous_;

		};
	}
}

