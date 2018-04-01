/*
* client_node.h
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

#include "reply.h"
#include "request_parser.h"
//#include "user_node.h"

#define CFTPSERVER_TRANSFER_BUFFER_SIZE (8 * 1024)
#define CFTPSERVER_MAX_PARAM_LEN			2000


namespace ftp {
	namespace server {

		//数据传输模式
		enum class ftp_data_mode
		{
			mode_none,
			mode_pasv,
			mode_port
		};

		//数据传输状态
		enum class ftp_client_status
		{
			status_waiting = 0,
			status_listing,
			status_uploading,
			status_downloading
		};
		//数据端口范围
		typedef	struct tag_data_port_range
		{
			int num;
			unsigned short int start;

		} data_port_range;

		class user_node;
		class user_manager;

		typedef shared_ptr<user_node> sp_user_node;


		/* The Structure which will be allocated for each Client. */
		class client_node : public enable_shared_from_this<client_node>
		{
		public:
			client_node();
			~client_node();
			//连接开始
			bool start(boost::asio::ip::tcp::socket& ctrl_socket);
			//连接结束
			bool end();

			//ftp命令处理函数
			bool process_user_cmd(bool is_allow_anonymous, user_manager& user_manager, wstring cmd_arg, reply& ftpreply);
			bool process_password_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_site_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_rein_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_type_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_port_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_pasv_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_list_cmd(const t_command& cmd, reply& ftpreply);
			bool process_cwd_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_mdtm_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_cdup_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_abor_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_rest_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_retr_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_stor_cmd(const t_command& cmd, reply& ftpreply);
			bool process_size_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_del_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_rnfr_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_rnto_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_mkd_cmd(wstring cmd_arg, reply& ftpreply);
			bool process_rmd_cmd(wstring cmd_arg, reply& ftpreply);

			//获取字符编码
			codetype get_code_type()
			{
				return  code_type_;
			}
			//设置字符编码
			void set_code_type(codetype type)
			{
				code_type_ = type;
			}
			//获取登录状态
			bool get_is_logged()
			{
				return is_logged_;
			}
			//获取当前连接socket
			boost::asio::ip::tcp::socket* get_ctrl_socket()
			{
				return ctrl_socket_;
			}
			//设置当前连接socket
			void set_ctrl_socket(boost::asio::ip::tcp::socket* ctrl_socket)
			{
				ctrl_socket_ = ctrl_socket;
			}
			//设置服务器ip
			void set_server_ip(unsigned long ip)
			{
				server_ip_ = ip;
			}
			//设置客户端ip
			void set_client_ip(unsigned long ip)
			{
				client_ip_ = ip;
			}
			//获取当前目录
			const std::wstring& get_current_directory()
			{
				return current_directory_;
			}
			//设置当前目录
			void set_current_directory(const std::wstring& dir)
			{
				current_directory_ = dir;
			}
			request_parser &get_parser()
			{
				return request_parser_;
			}
		private:
			request_parser request_parser_;

			boost::asio::io_service io_service_;

			boost::asio::ip::tcp::socket* ctrl_socket_;

			shared_ptr<boost::asio::ip::tcp::socket> data_socket_;

			shared_ptr<boost::asio::ip::tcp::acceptor> data_acceptor_;

			mutex port_lock_;

			//in host byte order
			unsigned long server_ip_;
			unsigned long client_ip_;

			bool binary_mode_;

			codetype code_type_;

			struct
			{

#ifdef __USE_FILE_OFFSET64
				long long restart_at_;
#else
				uintmax_t restart_at_;
#endif
				std::wstring transfer_path_;


			} current_transfer_;

			/*****************************************
			* USER
			*****************************************/

			sp_user_node user_;

			bool is_logged_;
			bool is_anonymous_;

			ftp_data_mode cur_data_mode_;
			ftp_client_status cur_status_;

			unsigned long data_ip_;
			unsigned short data_port_;

			std::wstring rename_from_path_;

			std::wstring current_directory_;
			/******************************************
			* OTHER
			******************************************/

			bool is_ctrl_canal_open_;

			const wchar_t *month_string[12];

			mutex ctrl_socket_lock_;

			void  list_thread(const t_command cmd);
			void  store_thread();
			void  retrieve_thread();
			/* Open the Data Canal in order to transmit data.
			Returns:
			-on success: true.
			-on error: false.
			*/
			bool open_data_connection();
			bool send_reply(const string& reply_string, bool is_ctrl = true);
			bool send_reply(const wstring& reply_string, bool is_ctrl = true);
			bool send_reply(reply& ftpreply, bool is_ctrl = true);
			bool send_reply(unsigned short status_code, const wstring& reply_string);

			bool shut_down_data_socket()
			{
				boost::system::error_code ec;
				data_socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
				return !ec;

			}
			bool close_data_socket()
			{
				return close_socket(data_socket_);

			}

			template <class T> bool open_socket(T& sock)
			{
				boost::system::error_code ec;
				sock->open(boost::asio::ip::tcp::v4(), ec);
				return !ec;
			}

			template <class T> bool close_socket(T& sock)
			{
				boost::system::error_code ec;
				sock->close(ec);
				return !ec;
			}

		};
		typedef shared_ptr<client_node> sp_client_node;

		typedef list<sp_client_node> client_list;
	}
}

