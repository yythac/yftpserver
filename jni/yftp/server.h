/*
* server.hpp 
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <string>
#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>
//#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "connection.h"
#include "ftp_server.h"


namespace ftp {
namespace server {

/// The top-level class of the ftp server.
class YFtpServer final
	: public ftp_server ,private boost::noncopyable
{
public:

	YFtpServer();
  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit YFtpServer(const std::wstring& address, const std::wstring& port,
      const std::wstring& doc_root, std::size_t thread_pool_size);

  /// Run the server's io_service loop.
  void run();

  void stop()
  {
	  handle_stop();
  }

  /***************************************
  * START / STOP
  ***************************************/

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
  bool StartListening(unsigned long ulAddr=INADDR_ANY, unsigned short int usPort=0);

  /* Ask the Server to Stop Listening.
  Returns:
  -on success: true.
  -on error: false.
  */
  bool StopListening(void);

  /* Check if the Server is currently Listening.
  Returns:
  -true: if the Server is currently listening.
  -false: if the Server isn't currently listening.
  */
  bool IsListening(void) {

	  return this->bIsListening;

  }

  bool IsStoped(void)
  {
	  return is_stopped;
	  return this->io_service_.stopped();
  }
  /****************************************
  * CONFIG
  ****************************************/

  /* Get the TCP Port on which CFtpServer will listen for incoming clients.
  Arguments:
  Returns:
  -on success: the TCP-Port.
  -on error: 0.
  */
  unsigned short GetListeningPort(void) {

	  return this->usPort_;

  }
  void SetListeningPort( unsigned short port)
  {
	  this->usPort_ = port;
  }
private:

  /// Initiate an asynchronous accept operation.
  void start_accept();

  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();

  void init();

  /// The number of threads that will call io_service::run().
  std::size_t thread_pool_size_;

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// The signal_set is used to register for process termination notifications.
  boost::asio::signal_set signals_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  connection_ptr new_connection_;

  bool bIsListening;

  unsigned short int usPort_;

  bool is_stopped;
};

} // namespace server
} // namespace ftp

