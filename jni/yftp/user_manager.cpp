/*
* user_manager.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#include "stdafx.h"
#include "user_manager.h"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

namespace ftp {
	namespace server {

		user_manager::user_manager()
		{
		}


		user_manager::~user_manager()
		{
		}

		//设置用户密码
		//参数：
		//name:需要设置密码的用户名，空表示所有用户
		//priv:需要设置的用户密码
		//true:设置密码成功
		//false:没有找到用户或者其他情况
		bool user_manager::set_user_password(const std::wstring& name, const std::wstring& password)
		{
			bool ret = false;

			std::lock_guard<std::mutex> lock(user_lock_);

			for (auto user: ftp_user_list_)
			{
				if (name.empty() == true || user->get_user_name() == name)
				{
					ret = true;
					user->set_user_password(password);
					if (name.empty() == false)
						break;
				}
			}
			if (ret == false)
			{
				last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
			}
			return ret;

		}
		bool user_manager::proccess_home_directory(std::wstring& home_dir)
		{
			std::wstring temp(L"");

			if (home_dir.empty() == true || home_dir == L".")
			{

				get_app_dir(temp);

			}
			else
			{

				temp = home_dir;

			}

			if (temp.empty() == false)
				simplify_path(temp);

			if (temp.empty() == true)
			{
				last_error_.set_error_code(YFTP_ERROR_PATH_INVALID);
				return false;
			}

			file_status st;

			if (!get_file_status(temp, st) || !is_directory(st)) 
			{
				last_error_.set_error_code(YFTP_ERROR_PATH_INVALID);
				return false;
			}
			home_dir = temp;

			return true;
		}
		bool user_manager::get_user_home_directory(const std::wstring& name, std::wstring& home_dir)
		{
			std::lock_guard<std::mutex> lock(user_lock_);
			auto user = find_user(name);
			if (user != nullptr)
			{
				home_dir = user->get_home_directory();
				return true;
			}
			last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
			return false;

		}
		bool user_manager::set_user_home_directory(const std::wstring& name, const std::wstring& home_dir)
		{
			std::wstring temp(L"");

			temp = home_dir;
			if (proccess_home_directory(temp) == false)
			{
				return false;
			}
			{
				std::lock_guard<std::mutex> lock(user_lock_);
				auto user = find_user(name);
				if (user != nullptr)
				{
					user->set_home_directory(temp);
					return true;
				}
			}
			last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
			return false;

		}
		bool user_manager::add_user(const wstring& name, const wstring& password
			, const wstring& home_dir, unsigned char priv)
		{

			if (name.empty() == false )
			{

				std::wstring temp(L"");

				temp = home_dir;
				if (proccess_home_directory(temp) == false)
				{
					return false;
				}
				{
					std::lock_guard<std::mutex> lock(user_lock_);
					if (this->find_user(name) != nullptr)
					{
						last_error_.set_error_code(YFTP_ERROR_USER_EXIST);
						return false; // User name already exists.
					}
					sp_user_node user(new user_node);

					user->set_user_name(name);
					user->set_user_password(password);
					user->set_home_directory(temp);
					user->set_user_privilege(priv);

					add_user(user);
				}

				return true;
			}
			else
			{
				last_error_.set_error_code(YFTP_ERROR_PARAM_INVALID);
				return false;
			}
		}
		sp_client_node user_manager::find_client(boost::asio::ip::tcp::socket& ctrl_socket)
		{
			std::lock_guard<std::mutex> lock(user_lock_);

			sp_client_node client = nullptr;

			for (auto user: ftp_user_list_)
			{
				client = user->get_client_manager().find_client(ctrl_socket);
				if (client != nullptr)
				{
					break;
				}
			}

			return client;
		}
		bool user_manager::delete_user(const wstring& name)
		{
			std::lock_guard<std::mutex> lock(user_lock_);
			for (auto pos = ftp_user_list_.begin(); pos != ftp_user_list_.end(); ++pos)
			{
				if ((*pos)->get_user_name() == name)
				{
					(*pos)->get_client_manager().delete_all_client();
					ftp_user_list_.erase(pos);
					return true;
				}
			}

			last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);

			return false;
		}
		bool user_manager::set_user_privilege(const std::wstring& name, unsigned char priv)
		{
			bool ret = false;

			std::lock_guard<std::mutex> lock(user_lock_);
			for (auto user: ftp_user_list_)
			{
				if (name.empty() == true || user->get_user_name() == name)
				{
					ret = true;
					user->set_user_privilege(priv);
					if (name.empty() == false)
						break;
				}
			}
			if (ret == false)
			{
				last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
			}
			return ret;
		}
		int user_manager::get_user_max_client(const std::wstring& name)
		{
			std::lock_guard<std::mutex> lock(user_lock_);
			auto user = find_user(name);
			if (user != nullptr)
			{
				return user->get_max_client_num();

			}
			last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
			return -1;

		}
		bool user_manager::set_user_max_client(const std::wstring& name, int max_client)
		{
			std::lock_guard<std::mutex> lock(user_lock_);
			auto user = find_user(name);
			if (user != nullptr)
			{
				user->set_max_client_num(max_client);
				return true;
			}
			last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
			return false;
		}
		unsigned char user_manager::get_user_privilege(const std::wstring& name)
		{
			std::lock_guard<std::mutex> lock(user_lock_);

			auto user = find_user(name);
			if (user != nullptr)
			{
				return user->get_user_privilege();

			}
			last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
			return 0;

		}
		bool  user_manager::get_user_password(const std::wstring& name, std::wstring& password)
		{
			std::lock_guard<std::mutex> lock(user_lock_);
			auto user = find_user(name);
			if (user != nullptr)
			{
				password = user->get_user_password();
				return true;
			}
			last_error_.set_error_code(YFTP_ERROR_USER_NOT_FOUND);
			return false;
		}
	}
}