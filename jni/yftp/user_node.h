/*
* user_node.h
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#pragma once

#include <memory>
#include <string>
#include <list>
#include <mutex>

using namespace std;

#include "error_code.h"
#include "common.h"
#include "client_manager.h"

#define MAX_CLIENT_NUM_PER_USER		10

namespace ftp {
	namespace server {

#define MAX_CLIENT_NUM_PER_USER		10

		//ftp user privilege
		typedef enum class tag_user_privilege
		{

			read_file = 0x1,
			write_file = 0x2,
			delete_file = 0x4,
			list_dir = 0x8,
			create_dir = 0x10,
			delete_dir = 0x20,

		}user_privilege;

#define FTP_USER_PRIV_READ	((unsigned char)ftp::server::user_privilege::read_file|(unsigned char)ftp::server::user_privilege::list_dir)
#define FTP_USER_PRIV_WRITE ((unsigned char)ftp::server::user_privilege::write_file|(unsigned char)ftp::server::user_privilege::create_dir)
#define FTP_USER_PRIV_DEL	((unsigned char)ftp::server::user_privilege::delete_file|(unsigned char)ftp::server::user_privilege::delete_dir)
#define FTP_USER_PRIV_ALL	(FTP_USER_PRIV_READ|FTP_USER_PRIV_WRITE|FTP_USER_PRIV_DEL)


		/* Enum the Extra Commands a User can got. */
		enum class ftp_extra_cmd {

			extracmd_exec = 0x1,

		};

		/* The Structure which will be allocated for each User. */
		class user_node
		{

		public:

			user_node()
			{
				user_name_.clear();
				user_password_.clear();
				home_directory_.clear();
				user_privilege_ = (unsigned char)FTP_USER_PRIV_READ;
				is_enable_ = true;
				extra_cmd_ = 0;

				max_client_num_ = MAX_CLIENT_NUM_PER_USER;

			}
			bool add_client(sp_client_node client)
			{
				std::lock_guard<std::mutex> lock(client_lock_);

				if (max_client_num_ == -1 || client_manager_.get_client_num() < max_client_num_)
				{
					client_manager_.add_client(client);
					return true;
				}
				return false;
			}
			void set_is_enable(bool enable)
			{
				is_enable_ = enable;
			}
			bool is_enable()
			{
				return is_enable_;
			}
			client_manager& get_client_manager()
			{
				return client_manager_;
			}

			std::wstring& get_user_name()
			{
				return user_name_;
			}
			void set_user_name(const std::wstring& name)
			{
				user_name_ = name;
			}

			std::wstring& get_user_password()
			{
				return user_password_;
			}
			void set_user_password(const std::wstring& password)
			{
				user_password_ = password;
			}
			std::wstring& get_home_directory()
			{
				return home_directory_;
			}
			void set_home_directory(const std::wstring& dir)
			{
				home_directory_ = dir;
				if (*home_directory_.rbegin() != L'\\'
					&& *home_directory_.rbegin() != L'/')
				{
					home_directory_.append(L"/");
				}
			}

			unsigned char get_user_privilege()
			{
				return user_privilege_;
			}
			void set_user_privilege(unsigned char priv)
			{
				user_privilege_ = priv;
			}

			int get_max_client_num()
			{
				return max_client_num_;
			}
			void set_max_client_num(int max)
			{
				max_client_num_ = max;
			}

			unsigned char get_extra_cmd()
			{
				return extra_cmd_;
			}
			void set_extra_cmd(int cmd)
			{
				extra_cmd_ = cmd;
			}
		private:

			std::wstring user_name_;
			std::wstring user_password_;
			bool is_enable_;
			std::wstring home_directory_;
			unsigned char user_privilege_;

			int max_client_num_;

			unsigned char extra_cmd_;

			client_manager client_manager_;

			mutex client_lock_;
		};

		typedef shared_ptr<user_node> sp_user_node;

		typedef list<sp_user_node> user_list;
	}
}