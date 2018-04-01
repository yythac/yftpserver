/*
* server.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/
#include "stdafx.h"
#include "server.h"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace ftp {
	namespace server {

		YFtpServer::YFtpServer()
			: signals_(io_service_),
			acceptor_(io_service_),
			new_connection_()
		{

			init();
		}

		YFtpServer::YFtpServer(const std::wstring& address, const std::wstring& port,
			const std::wstring& doc_root, std::size_t thread_pool_size)
			: thread_pool_size_(thread_pool_size),
			signals_(io_service_),
			acceptor_(io_service_),
			new_connection_()
		{
			// Register to handle the signals that indicate when the server should exit.
			// It is safe to register for the same signal multiple times in a program,
			// provided all registration for the specified signal is made through Asio.
			init();

			// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
			boost::asio::ip::tcp::resolver resolver(io_service_);
			boost::asio::ip::tcp::resolver::query query(ws2s(address), ws2s(port));
			boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
			acceptor_.open(endpoint.protocol());
			acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor_.bind(endpoint);
			acceptor_.listen();

			start_accept();
		}
		void YFtpServer::init()
		{
			signals_.add(SIGINT);
			signals_.add(SIGTERM);
#if defined(SIGQUIT)
			signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
			signals_.async_wait(boost::bind(&YFtpServer::handle_stop, this));

			this->usPort_ = 21;

			this->bIsListening = false;

			is_stopped = true;

			thread_pool_size_ = (boost::thread::hardware_concurrency() * 2)+2;
		}
		void YFtpServer::run()
		{
			is_stopped = false;
			io_service_.reset();
			// Create a pool of threads to run all of the io_services.
			std::vector<shared_ptr<boost::thread> > threads;
			for (std::size_t i = 0; i < thread_pool_size_; ++i)
			{
				shared_ptr<boost::thread> thread(new boost::thread(
					boost::bind(&boost::asio::io_service::run, &io_service_)));
				threads.push_back(thread);
			}

			// Wait for all threads in the pool to exit.
			for (std::size_t i = 0; i < threads.size(); ++i)
				threads[i]->join();

			is_stopped = true;
		}

		void YFtpServer::start_accept()
		{
			new_connection_.reset(new connection(io_service_, *this));
			acceptor_.async_accept(new_connection_->socket(),
				boost::bind(&YFtpServer::handle_accept, this,
					boost::asio::placeholders::error));
		}

		void YFtpServer::handle_accept(const boost::system::error_code& e)
		{
			if (!e)
			{
				new_connection_->start();
				start_accept();
			}

		}

		void YFtpServer::handle_stop()
		{
			io_service_.stop();
		}
		/* Ask the Server to Start Listening on the TCP-Port supplied by SetPort().
		Arguments:
		-the Network Adress CFtpServer will listen on.
		Example:INADDR_ANY for all local interfaces.
		inet_addr( "127.0.0.1" ) for the TCP Loopback interface.
		-the TCP-Port on which CFtpServer will listen.
		Returns:
		-on success: true.
		-on error: false, the supplied Adress or TCP-Port may not be valid.
		*/
		bool YFtpServer::StartListening(unsigned long ulAddr, unsigned short int usPort)
		{
			try
			{
				if (ulAddr == INADDR_NONE || (usPort == 0 && usPort_== 0))
					return false;

				if (this->IsListening()) {

					if (StopListening() == false)
					{
						return false;
					}

				}

				boost::asio::ip::tcp::endpoint endpoint1(boost::asio::ip::address(boost::asio::ip::address_v4(ulAddr))
				,usPort == 0 ?usPort_: usPort);
				//boost::asio::ip::tcp::endpoint endpoint1(boost::asio::ip::address::from_string("127.0.0.1"), usPort);

				boost::asio::ip::tcp::resolver resolver(io_service_);
			//	boost::asio::ip::tcp::resolver::query query(ws2s(L"127.0.0.1"), ws2s(L"21"));
			//	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

				acceptor_.open(endpoint1.protocol());
				acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
				acceptor_.bind(endpoint1);
				acceptor_.listen();

				if(usPort != 0)
				{
					this->usPort_ = usPort;
				}

				this->bIsListening = true;

				start_accept();

			}
			catch (boost::system::system_error& e)
			{
				if (acceptor_.is_open())
				{
					boost::system::error_code ec;
					acceptor_.close(ec);
				}
			}
			return this->bIsListening;
		}

		/* Ask the Server to Stop Listening.
		Returns:
		-on success: true.
		-on error: false.
		*/
		bool YFtpServer::StopListening(void)
		{
			if (this->IsListening()) {

				boost::system::error_code ec;
				acceptor_.cancel(ec);

				acceptor_.close(ec);
				if (!ec)
				{

					this->bIsListening = false;
				}

			}
			return !this->bIsListening;
		}

	} // namespace server
} // namespace ftp
