/*
* connection.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#include "stdafx.h"
#include "connection.hpp"
#include <vector>
#include <boost/bind.hpp>


namespace ftp {
	namespace server {

		connection::connection(boost::asio::io_service& io_service,
			ftp_server& handler)
			: strand_(io_service),
			socket_(io_service),
			cmd_hander_(handler)
		{
		}

		boost::asio::ip::tcp::socket& connection::socket()
		{
			return socket_;
		}

		void connection::start()
		{
			reply ftpreply;

			bool ret = cmd_hander_.start_work(ftpreply);
			if (ftpreply.empty() == false)
			{
				boost::asio::async_write(socket_, ftpreply.to_buffer(),
					strand_.wrap(
						boost::bind(&connection::handle_write, shared_from_this(),
							boost::asio::placeholders::error)));
			}
			if (ret == true)
			{
				socket_.async_read_some(boost::asio::buffer(buffer_),
					strand_.wrap(
						boost::bind(&connection::handle_read, shared_from_this(),
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred)));
			}

		}

		void connection::handle_read(const boost::system::error_code& e,
			std::size_t bytes_transferred)
		{
			reply ftpreply;

			if (!e)
			{
				bool result;
				result = cmd_hander_.process_command(socket_,
				 buffer_.data(), bytes_transferred, ftpreply);

				if (ftpreply.empty() == false)
				{
					boost::asio::async_write(socket_, ftpreply.to_buffer(),
						strand_.wrap(
							boost::bind(&connection::handle_write, shared_from_this(),
								boost::asio::placeholders::error)));
				}

				if (result == true)
				{
					socket_.async_read_some(boost::asio::buffer(buffer_),
						strand_.wrap(
							boost::bind(&connection::handle_read, shared_from_this(),
								boost::asio::placeholders::error,
								boost::asio::placeholders::bytes_transferred)));

				}
				else
				{
					cmd_hander_.end_work(socket_);
				}
			}
			else
			{
				cmd_hander_.end_work(socket_);
			}

			// If an error occurs then no new asynchronous operations are started. This
			// means that all shared_ptr references to the connection object will
			// disappear and the object will be destroyed automatically after this
			// handler returns. The connection class's destructor closes the socket.
		}

		void connection::handle_write(const boost::system::error_code& e)
		{
			if (!e)
			{
				// Initiate graceful connection closure.
			//    boost::system::error_code ignored_ec;
			 //   socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);

			}

			// No new asynchronous operations are started. This means that all shared_ptr
			// references to the connection object will disappear and the object will be
			// destroyed automatically after this handler returns. The connection class's
			// destructor closes the socket.
		}

	} // namespace server
} // namespace ftp
