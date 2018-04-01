#pragma once

/*
* user_manager.h :: ftp user manager class header file
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#include <memory>
#include <string>
#include <list>
#include <mutex>

using namespace std;

#include "user_node.h"

namespace ftp {
	namespace server {

		class user_manager
		{
		public:
			user_manager();
			~user_manager();

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
				
			//删除用户
			//参数：
			//name：需要删除的用户名
			//返回：
			//true:删除成功
			//false:没有找到用户或其他情况
			bool delete_user(const wstring& name);

			//设置用户权限
			//参数：
			//name:需要设置权限的用户名，空表示所有用户
			//priv:需要设置的用户权限
			//true:设置权限成功
			//false:没有找到用户或者其他情况
			bool set_user_privilege(const std::wstring& name, unsigned char priv);

			//设置一个用户能同时登录的最大连接数
			//参数:
			//name:需要设置参数的用户名
			//返回:
			//true:设置成功
			//fasle:设置失败
			bool set_user_max_client(const std::wstring& name, int max_client);

			//获取一个用户能同时登录的最大连接数
			//参数:
			//name:需要获取参数的用户名
			//返回:
			//>= 0:用户能同时登录的最大连接数
			//< 0:获取失败
			int get_user_max_client(const std::wstring& name);

			//获取用户权限
			//参数:
			//name:
			//返回:
			//>0: 该用户的权限
			//0: 获取权限失败
			unsigned char get_user_privilege(const std::wstring& name);
			//获取用户登录密码
			bool get_user_password(const std::wstring& name, std::wstring& password);
			//获取用户ftp目录
			bool get_user_home_directory(const std::wstring& name, std::wstring& home_dir);

			//设置用户密码
			//参数：
			//name:需要设置密码的用户名，空表示所有用户
			//priv:需要设置的用户密码
			//true:设置密码成功
			//false:没有找到用户或者其他情况
			bool set_user_password(const std::wstring& name, const std::wstring& password);
			//设置用户起始目录
			//参数：
			//name:需要设置起始目录的用户名
			//priv:需要设置的起始目录
			//true:设置起始目录成功
			//false:没有找到用户或者其他情况
			bool set_user_home_directory(const std::wstring& name, const std::wstring& home_dir);

			bool proccess_home_directory(std::wstring& home_dir);

			sp_client_node find_client(boost::asio::ip::tcp::socket& ctrl_socket);

			//查找用户
			//参数：
			//name：需要查找的用户名
			//返回：
			//sp_user_node:找到的用户信息
			//nullptr:没有找到用户
			sp_user_node find_user(const wstring& name)
			{
				for (auto user: ftp_user_list_)
				{
					if (user->get_user_name() == name)
					{
						return user;
					}
				}
				return nullptr;
			}

			//添加用户
			//参数：
			//user：需要添加的用户信息结构
			//返回：	
			//无
			void add_user(const sp_user_node& user)
			{
				ftp_user_list_.push_back(user);
			}
			//删除所有用户
			void delete_all_user(void)
			{
				std::lock_guard<std::mutex> lock(user_lock_);
				auto pos = ftp_user_list_.begin();

				while (pos != ftp_user_list_.end())
				{
					(*pos)->get_client_manager().delete_all_client();
					pos = ftp_user_list_.erase(pos);
				}
			}
			//获取错误码
			int get_last_error()
			{
				return last_error_.get_error_code();
			}

#ifdef CFTPSERVER_USE_EXTRACMD

			/* Enum the Extra Commands a User can got. */
			enum class ftp_extra_cmd {

				ExtraCmd_EXEC = 0x1,

			};

			/* Set the supported Extra-Commands of a User.
			Arguments:
			-user name.
			-the user's Extra-Commands concatenated with the bitwise inclusive
			binary operator "|".
			Returns:
			-on success: true.
			-on error: false.
			*/
			bool set_user_extra_cmd(const std::wstring& name, unsigned char extra_cmd)
			{
				std::lock_guard<std::mutex> lock(user_lock_);
				auto user = find_user(name);
				if (user != nullptr)
				{
					user->set_extra_cmd(extra_cmd);
					return true;
				}
				last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
				return false;
			}

			/* Get the supported Extra-Commands of a User.
			Arguments:
			-user name.
			Returns:
			-on succes: the user's Extra-Commands concatenated with the bitwise
			inclusive binary operator "|".
			-on error: 0.
			*/
			unsigned char get_user_extra_cmd(const std::wstring& name)
			{
				std::lock_guard<std::mutex> lock(user_lock_);
				auto user = find_user(name);
				if (user != nullptr)
				{
					return user->getextra_cmd();
				}
				last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
				return 0;

			}

#endif

		private:
			//ftp用户列表
			user_list	ftp_user_list_;

			//错误处理
			error_manager last_error_;

			mutex user_lock_;

		};

	}
}