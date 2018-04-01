/*
* client_manager.h :: ftp client manager class header file
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

#include <boost/asio.hpp>

using namespace std;
#include "error_code.h"
#include "client_node.h"

namespace ftp {
	namespace server {

		class client_manager
		{
		public:
			client_manager();
			~client_manager();

			//查找客户端连接
			sp_client_node find_client(boost::asio::ip::tcp::socket& ctrl_socket);
			//增加一个新的客户端连接
			sp_client_node add_client( boost::asio::ip::tcp::socket& ctrl_sock);
			void add_client(sp_client_node client)
			{
				std::lock_guard<std::mutex> lock(client_lock_);
				ftp_client_list_.push_back(client);
			}
			//删除一个客户端连接
			void delete_client(sp_client_node client);

			//删除所有客户端连接
			void delete_all_client(void);

			//获取客户端连接总数
			int get_client_num()
			{
				return ftp_client_list_.size();
			}

			//获取错误码
			int get_last_error()
			{
				return last_error_.get_error_code();
			}

		private:
			//客户端连接列表
			client_list	ftp_client_list_;

			//错误处理
			error_manager last_error_;

			mutex client_lock_;
		};

	}
}