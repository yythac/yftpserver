/*
* client_manager.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#include "stdafx.h"
#include "client_manager.h"

namespace ftp {
	namespace server {


		client_manager::client_manager()
		{

		}


		client_manager::~client_manager()
		{

		}
		sp_client_node client_manager::find_client(YCOMMON::YSERVER::i_ycommon_socket& ctrl_socket)
		{
			std::lock_guard<std::mutex> lock(client_lock_);

			for (auto client: ftp_client_list_)
			{
#ifdef SERVER_APP
				if (client->get_ctrl_socket()->i_native_handle() == ctrl_socket.i_native_handle())
#else
				if (client->get_ctrl_socket()->native_handle() == ctrl_socket.native_handle())
#endif
				{
					return client;
				}
			}
			return nullptr;
		}
		sp_client_node client_manager::add_client(YCOMMON::YSERVER::i_ycommon_socket& ctrl_sock)
		{
#ifdef SERVER_APP
			if (((boost::asio::ip::tcp::socket*)(ctrl_sock.i_lowest_layer()))->is_open() == true)
#else
			if (ctrl_sock.is_open() == true)
#endif
			{

				sp_client_node new_client(new client_node);

				new_client->set_ctrl_socket(&ctrl_sock);
#ifdef SERVER_APP
				new_client->set_server_ip(((boost::asio::ip::tcp::socket*)(ctrl_sock.i_lowest_layer()))->local_endpoint().address().to_v4().to_ulong());
				new_client->set_server_ip(((boost::asio::ip::tcp::socket*)(ctrl_sock.i_lowest_layer()))->remote_endpoint().address().to_v4().to_ulong());
#else
				new_client->set_server_ip(ctrl_sock.local_endpoint().address().to_v4().to_ulong());
				new_client->set_server_ip(ctrl_sock.remote_endpoint().address().to_v4().to_ulong());

#endif

				new_client->set_code_type(codetype::utf8);

				new_client->set_current_directory(L"/");


				{
					std::lock_guard<std::mutex> lock(client_lock_);

					ftp_client_list_.push_back(new_client);

				}

				return new_client;

			}
			return nullptr;

		}

		void client_manager::delete_client(sp_client_node client)
		{

			assert(client != nullptr);

			if (client == nullptr)
			{
				return;
			}
			{
				std::lock_guard<std::mutex> lock(client_lock_);

				for (auto pos = ftp_client_list_.begin(); pos != ftp_client_list_.end(); ++pos)
				{
					if ((*pos) == client)
					{
						ftp_client_list_.erase(pos);

						break;
					}
				}
			}

		}

		void client_manager::delete_all_client(void)
		{

			std::lock_guard<std::mutex> lock(client_lock_);
			ftp_client_list_.clear();

		}
	}
}