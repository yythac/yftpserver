/*
* client_node.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#include "stdafx.h"
#include "user_manager.h"
#include "client_node.h"

#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem/fstream.hpp>


namespace ftp {
	namespace server {

		client_node::client_node()
		{
			int i = 0;
			this->month_string[i++] = L"Jan"; this->month_string[i++] = L"Feb";
			this->month_string[i++] = L"Mar"; this->month_string[i++] = L"Apr";
			this->month_string[i++] = L"May"; this->month_string[i++] = L"Jun";
			this->month_string[i++] = L"Jul"; this->month_string[i++] = L"Aug";
			this->month_string[i++] = L"Sep"; this->month_string[i++] = L"Oct";
			this->month_string[i++] = L"Nov"; this->month_string[i++] = L"Dec";

			this->data_port_range_.start = 10000;
			this->data_port_range_.num = 10200;

			data_acceptor_ = nullptr;
			data_socket_ = nullptr;
			ctrl_socket_ = nullptr;

			rename_from_path_.clear();
			current_directory_ = L"/";
			data_ip_ = 0;
			data_port_ = 0;
			cur_data_mode_ = ftp_data_mode::mode_none;
			cur_status_ = ftp_client_status::status_waiting;
			is_logged_ = false;
			is_anonymous_ = false;
			user_ = nullptr;
			code_type_ = codetype::utf8;
			binary_mode_ = true;
			server_ip_ = client_ip_ = 0;

			is_ctrl_canal_open_ = false;

			current_transfer_.restart_at_ = 0;
			current_transfer_.transfer_path_.clear();
		}


		client_node::~client_node()
		{

		}
		bool client_node::start(boost::asio::ip::tcp::socket& ctrl_socket)
		{
			is_ctrl_canal_open_ = true;

			ctrl_socket_ = &ctrl_socket;

			server_ip_ = ctrl_socket.local_endpoint().address().to_v4().to_ulong();
			client_ip_ = ctrl_socket.remote_endpoint().address().to_v4().to_ulong();
			
			return true;
		}
		bool client_node::end()
		{
			std::lock_guard<std::mutex> lock(ctrl_socket_lock_);

			is_ctrl_canal_open_ = false;
			user_->get_client_manager().delete_client(shared_from_this());

			return true;
		}
		bool client_node::send_reply(const string& reply_string, bool is_ctrl)
		{

			if (reply_string.empty() == false)
			{
				boost::system::error_code ec;

				boost::asio::write(is_ctrl == true ? *ctrl_socket_ : *data_socket_, boost::asio::buffer(reply_string.c_str(), reply_string.length()), ec);

				return !ec;
			}

			return true;

		}

		bool client_node::send_reply(reply& ftpreply, bool is_ctrl)
		{

			return send_reply(ftpreply.to_string(), is_ctrl);
		}

		bool client_node::send_reply(const wstring& reply_string, bool is_ctrl)
		{
			std::string temp_string("");

			if (code_type_ == codetype::gbk)
			{
				temp_string = ws2s(reply_string);
			}
			else
			{
				temp_string = ws2utf8(reply_string);
			}

			return send_reply(temp_string, is_ctrl);
		}
		bool client_node::send_reply(unsigned short status_code, const wstring& reply_string)
		{
			reply tmp_reply(status_code, reply_string, code_type_);
			
			return send_reply(tmp_reply, true) ;

		}
		bool client_node::open_data_connection()
		{
			boost::system::error_code ec;

			if (cur_data_mode_ == ftp_data_mode::mode_pasv)
			{
				if (data_socket_ == nullptr)
					data_socket_ = make_shared<boost::asio::ip::tcp::socket>(io_service_);
				if (data_socket_ == nullptr)
				{
					return false;
				}

				data_acceptor_->accept(*data_socket_, ec);
				close_socket(data_acceptor_);
				return !ec;

			}
			else if (cur_data_mode_ == ftp_data_mode::mode_port)
			{

				if (data_socket_ == nullptr)
					data_socket_ = make_shared<boost::asio::ip::tcp::socket>(io_service_);
				if (data_socket_ == nullptr)
				{
					return false;
				}
				if (open_socket(data_socket_) == false)
				{
					return false;
				}

				boost::asio::ip::tcp::endpoint endpoint;

				data_socket_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);

#ifdef __ECOS
				endpoint.address = boost::asio::ip::address(boost::asio::ip::address_v4::any());

#else
				endpoint.address(boost::asio::ip::address(boost::asio::ip::address_v4(server_ip_)));

#endif
				endpoint.port((unsigned short)(this->data_port_range_.start + (rand() % this->data_port_range_.num)));


				data_socket_->bind(endpoint, ec);
				if (ec)
				{
					return false;
				}

				endpoint.address(boost::asio::ip::address(boost::asio::ip::address_v4(data_ip_)));
				endpoint.port(data_port_);

				data_socket_->connect(endpoint, ec);

				return !ec;

			}
			return false;

		}
		//´¦Àíftp ¡°USER" ÃüÁî
		bool client_node::process_user_cmd(bool is_allow_anonymous,user_manager& user_manager,wstring cmd_arg, reply& ftpreply)
		{
			bool ret = true;

			if (is_logged_ == true)
			{
				is_logged_ = false;
				user_->get_client_manager().delete_client(shared_from_this());
				user_ = nullptr;

			}
			if (cmd_arg.empty() == true)
			{

				ftpreply.create(501, L"Invalid number of arguments.");

			}
			else
			{

				if (is_allow_anonymous && (cmd_arg == L"anonymous" || cmd_arg == L"ftp"))
				{
					sp_user_node user = user_manager.find_user(L"anonymous");
					if (user != nullptr)
					{
						
						if (user->add_client(shared_from_this()) == true)
						{
							is_logged_ = true;
							is_anonymous_ = true;
							user_ = user;

							ftpreply.create(230, L"User Logged In.");

						}
						else
						{
							ftpreply.create(421, L"Too many users logged in for this account.");
							ret = false;
						}

					}

				}
				else
				{

					user_ = user_manager.find_user(cmd_arg);
					if (user_ && user_->is_enable() == false)
						user_ = nullptr;
					if (user_ != nullptr)
					{
						if (user_->add_client(shared_from_this()) == true)
						{
							ftpreply.create(331, L"Password required for this user.");
						}
						else
						{
							ftpreply.create(550, L"Internal Error-Can't add client.");
							ret = false;
						}
					}
					else
					{
						ftpreply.create(331, L"Password required for this user.");
					}
				}

			}
			return ret;
		}

		bool client_node::process_password_cmd(wstring cmd_arg, reply& ftpreply)
		{
			bool ret = true;

			if (is_anonymous_)
			{
				is_logged_ = true;
				ftpreply.create(230, L"User Logged In.");
			}
			else 
			{

#ifdef CFTPSERVER_ANTIBRUTEFORCING
				boost::this_thread::sleep_for(boost::chrono::milliseconds(check_pass_delay_));
#endif

				if (cmd_arg.empty() == false)
				{

					if (user_ && user_->is_enable() &&
						cmd_arg == user_->get_user_password())
					{

						//if (user_->add_client(shared_from_this()) == true)
						{

							is_logged_ = true;
							ftpreply.create(230, L"User Logged In.");

						}
						/*else
						{
							ftpreply.create(421, L"Too many users logged in for this account.");
							ret = false;
						}*/

					}
					else
					{
						if (user_ != nullptr)
						{
							user_->get_client_manager().delete_client(shared_from_this());
							user_ = nullptr;
						}
						ftpreply.create(530, L"Please login with valid USER and PASS.");
#ifdef CFTPSERVER_ANTIBRUTEFORCING
						ret = false;
#endif

					}

				}
				else
				{
					if (user_ != nullptr)
					{
						user_->get_client_manager().delete_client(shared_from_this());
						user_ = nullptr;
					}
					ftpreply.create(501, L"Invalid number of arguments.");
				}

			}
			return ret;
		}
		bool client_node::process_site_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (cmd_arg.empty() == false)
			{

				unsigned char ucExtraCmd = user_->get_extra_cmd();

				if (cmd_arg.substr(0, 5) == L"EXEC ")
				{

					if ((ucExtraCmd & (unsigned char)ftp_extra_cmd::extracmd_exec) == (unsigned char)ftp_extra_cmd::extracmd_exec) {

						if (cmd_arg.length() > 5)
							system(ws2s(cmd_arg.substr(5)).c_str());
						ftpreply.create(200, L"SITE EXEC Successful.");

					}
					else
					{
						ftpreply.create(550, L"Permission denied.");
					}

				}

			}
			else
			{
				ftpreply.create(501, L"Syntax error in arguments.");
			}
			return true;
		}
		bool client_node::process_rein_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (user_)
			{
				user_->get_client_manager().delete_client(shared_from_this());
				user_ = nullptr;

			}
			is_logged_ = false;
			binary_mode_ = true;
			is_anonymous_ = false;
			current_directory_ = L"/";
			cur_data_mode_ = ftp_data_mode::mode_none;
			rename_from_path_.clear();
			// /!\ the current transfert must not be stopped
			ftpreply.create(200, L"Reinitialize Successful.");

			return true;
		}
		bool client_node::process_type_cmd(wstring cmd_arg, reply& ftpreply)
		{

			if (cmd_arg.empty() == false)
			{

				if (cmd_arg == L"A")
				{

					binary_mode_ = false; // Infact, ASCII mode isn't supported..
					ftpreply.create(200, L"ASCII transfer mode active.");

				}
				else if (cmd_arg == L"I")
				{

					binary_mode_ = true;
					ftpreply.create(200, L"Binary transfer mode active.");

				}
				else
				{
					ftpreply.create(550, L"Error - unknown binary mode.");
				}

			}
			else
			{
				ftpreply.create(501, L"Invalid number of arguments.");
			}
			return true;
		
		}
		bool client_node::process_port_cmd(wstring cmd_arg, reply& ftpreply)
		{

			if (cur_status_ != ftp_client_status::status_waiting)
			{
				ftpreply.create(425, L"You're already connected.");
				return true;
			}

			unsigned int iIp1 = 0, iIp2 = 0, iIp3 = 0, iIp4 = 0, iPort1 = 0, iPort2 = 0;

			if (cmd_arg.empty() == false
				&& swscanf(cmd_arg.c_str(), L"%u,%u,%u,%u,%u,%u", &iIp1, &iIp2, &iIp3, &iIp4, &iPort1, &iPort2) == 6
				&& iIp1 <= 255 && iIp2 <= 255 && iIp3 <= 255 && iIp4 <= 255
				&& iPort1 <= 255 && iPort2 <= 255 && (iIp1 | iIp2 | iIp3 | iIp4) != 0
				&& (iPort1 | iPort2) != 0)
			{

				if (cur_data_mode_ != ftp_data_mode::mode_none)
				{

					close_socket(data_socket_);
					cur_data_mode_ = ftp_data_mode::mode_none;

				}
				if (data_socket_ == nullptr)
					data_socket_ = make_shared<boost::asio::ip::tcp::socket>(io_service_);
				if (data_socket_ == nullptr)
				{
					ftpreply.create(451, L"451 Internal error - No socket available.");
					return true;
				}
				char client_ip[32];
				sprintf(client_ip, "%u.%u.%u.%u", iIp1, iIp2, iIp3, iIp4);
				unsigned long port_ip = ntohl(inet_addr(client_ip));

#ifdef CFTPSERVER_STRICT_PORT_IP
				if (ulPortIp == client->client_ip
					|| client->client_ip == inet_addr("127.0.0.1")
					)
				{

#endif
					cur_data_mode_ = ftp_data_mode::mode_port;
					data_ip_ = port_ip;
					data_port_ = (unsigned short)((iPort1 * 256) + iPort2);
					ftpreply.create(200, L"PORT command successful.");
#ifdef CFTPSERVER_STRICT_PORT_IP

				}
				else
				{
					ftpreply.create(200, L"PORT address does not match originator.");
				}
#endif


			}
			else
			{
				ftpreply.create(501, L"Syntax error in arguments.");
			}

			return true;
		}
		bool client_node::process_pasv_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (cur_status_ != ftp_client_status::status_waiting)
			{
				ftpreply.create(425, L"You're already connected.");
				return true;
			}

			boost::system::error_code ec;

			if (cur_data_mode_ != ftp_data_mode::mode_none)
			{

				close_socket(data_acceptor_);
				cur_data_mode_ = ftp_data_mode::mode_none;

			}
			if(data_acceptor_ == nullptr)
				data_acceptor_ = make_shared<boost::asio::ip::tcp::acceptor>(io_service_);
			if (data_acceptor_ == nullptr)
			{
				ftpreply.create(451,L"451 Internal error - No socket available.");
				return true;
			}
			if (open_socket(data_acceptor_) == false)
			{
				ftpreply.create(451, L"451 Internal error - open socket fail.");
				return true;
			}

			boost::asio::ip::tcp::endpoint endpoint;

			data_acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true),ec);

#ifdef __ECOS
			endpoint.address(boost::asio::ip::address(boost::asio::ip::address_v4::any()));

#else
			endpoint.address(boost::asio::ip::address(boost::asio::ip::address_v4(server_ip_)));

#endif

			int checked_port_count = 0;
			do 
			{

				if (checked_port_count >= data_port_range_.num) 
				{

					checked_port_count = -1;
					close_socket(data_acceptor_);
					break;

				}
				checked_port_count++;
				data_port_ = (unsigned short)(data_port_range_.start + (rand() % data_port_range_.num));
				endpoint.port( data_port_);

			} while (data_acceptor_->bind(endpoint,ec) 
				|| data_acceptor_->listen(1,ec));

			if (checked_port_count == -1) 
			{

				ftpreply.create(451,L"Internal error - No more data port available.");
				return true;

			}

			unsigned int data_port2 = data_port_ % 256;
			unsigned int data_port1 = (data_port_ - data_port2) / 256;
			unsigned long ulIp = server_ip_;

			ftpreply.create(227,(boost::wformat(L"Entering Passive Mode (%lu,%lu,%lu,%lu,%u,%u).")
				% ((ulIp >> 24) & 255) % ((ulIp >> 16) & 255)
				% ((ulIp >> 8) & 255) % (ulIp & 255) % data_port1 % data_port2).str());

			cur_data_mode_ = ftp_data_mode::mode_pasv;

			return true;
		}
		void  client_node::list_thread(const t_command cmd)
		{
			reply ftpreply;

			bool is_ctrl = true;

			if (cmd.id != commands::STAT) 
			{
				send_reply(150,L"Opening data connection for directory list.");
				is_ctrl = false;
				this->cur_status_ = ftp_client_status::status_listing;

			}
			else
			{
				send_reply(L"213-Status follows:");
			}


			bool opt_a = false, opt_d = false, opt_F = false, opt_l = false;

			// Extract parametres
			const wchar_t *pszArg = cmd.args.c_str();
			if (cmd.args.empty() == false) 
			{

				while (*pszArg == L'-') 
				{

					while (pszArg++ && iswalnum(*pszArg)) 
					{

						switch (*pszArg) 
						{

						case L'a':
							opt_a = true;
							break;
						case L'd':
							opt_d = true;
							break;
						case L'F':
							opt_F = true;
							break;
						case L'l':
							opt_l = true;
							break;

						}

					}
					while (iswspace(*pszArg))
						pszArg++;

				}

			}
			boost::filesystem::file_status stDir;
			boost::system::error_code ec;

			std::wstring szPath = build_path(user_->get_home_directory(), this->current_directory_, pszArg);

			if (szPath.empty() == false)
			{
			//	LOGI("List CMD::begin dir...%s", ws2s(szPath).c_str());
				if (get_file_status(szPath, stDir) == true)
				{

					struct tm *t;
					std::wstring szYearOrHour(L"");

					std::wstring strBuf(L"");

					if (opt_d || is_directory(stDir) == false) 
					{

						boost::filesystem::perms tmp_perms = stDir.permissions();
						uintmax_t tmp_filesize = boost::filesystem::file_size(boost::filesystem::path(szPath), ec);
						if (ec)
						{
							tmp_filesize = 0;
						}

						time_t last = boost::filesystem::last_write_time(boost::filesystem::path(szPath), ec);

						t = gmtime((time_t *)&last); // UTC Time
						if (time(nullptr) - last > 180 * 24 * 60 * 60) {

							szYearOrHour = boost::str(boost::wformat(L"%5d") % (t->tm_year + 1900));

						}
						else
						{
							szYearOrHour = boost::str(boost::wformat(L"%02d:%02d") % t->tm_hour % t->tm_min);
						}


						strBuf = boost::str(boost::wformat(
#ifdef __USE_FILE_OFFSET64

							L"%c%c%c%c------ 1 user group %14lli %s %2d %s %s%s\r\n"
#else
							L"%c%c%c%c------ 1 user group %14li %s %2d %s %s%s\r\n"
#endif
							) % ((is_directory(stDir) == true) ? L'd' : L'-')
							% ((tmp_perms  & boost::filesystem::owner_read) == boost::filesystem::owner_read ? L'r' : L'-')
							% ((tmp_perms & boost::filesystem::owner_write) == boost::filesystem::owner_write ? L'w' : L'-')
							% ((tmp_perms & boost::filesystem::owner_exe) == boost::filesystem::owner_exe ? L'x' : L'-')
							% tmp_filesize
							% month_string[t->tm_mon] % t->tm_mday % szYearOrHour
							% ((cmd.id == commands::STAT) ? szPath.substr(user_->get_home_directory().length()) : L".")
							% (opt_F ? L"/" : L""));
						send_reply(strBuf.c_str(), is_ctrl);

					}
					else
					{

						for (boost::filesystem::directory_iterator iter(boost::filesystem::path(szPath), ec)
							; iter != boost::filesystem::directory_iterator(); iter.increment(ec))
						{

							if (ec)
							{
								break;
							}
							if (cmd.id == commands::NLST) 
							{
								
								strBuf = boost::str(boost::wformat(L"%s\r\n") % (iter->path().wstring().substr(user_->get_home_directory().length())));

								send_reply(strBuf.c_str(), is_ctrl);

							}
							else
							{
								if (iter->path().wstring()[0] != L'.' || opt_a)
								{

									//	LOGI("begin getdir...2");
									boost::filesystem::file_status tmp_status = iter->status(ec);
									if (ec)
									{
										continue;
									}
									boost::filesystem::perms tmp_perms = tmp_status.permissions();
									
									uintmax_t tmp_filesize = 0;
									if (is_regular_file(tmp_status))
									{

										tmp_filesize = file_size(iter->path(), ec);
										if (ec)
										{
											tmp_filesize = 0;
										}
									}

									time_t last = last_write_time(iter->path(), ec);
									if (ec)
									{
										last = 0;
									}

									t = gmtime((time_t *)&last); // UTC Time

									if (time(nullptr) - last > 180 * 24 * 60 * 60)
									{

										szYearOrHour = boost::str(boost::wformat(L"%5d") % (t->tm_year + 1900));

									}
									else
									{

										szYearOrHour = boost::str(boost::wformat(L"%02d:%02d") % t->tm_hour % t->tm_min);
									}
									//	LOGI("begin getdir...4:%d",t->tm_mon);

									strBuf = boost::str(boost::wformat(
#ifdef __USE_FILE_OFFSET64
										L"%c%c%c%c%c%c%c%c%c%c 1 user group %14lld %s %2d %s %s%s\r\n"
#else
										L"%c%c%c%c%c%c%c%c%c%c 1 user group %14li %s %2d %s %s%s\r\n"
#endif
										) % (is_directory(tmp_status) ? L'd' : L'-')
										% ((tmp_perms  & boost::filesystem::owner_read) == boost::filesystem::owner_read ? L'r' : L'-')
										% ((tmp_perms & boost::filesystem::owner_write) == boost::filesystem::owner_write ? L'w' : L'-')
										% ((tmp_perms & boost::filesystem::owner_exe) == boost::filesystem::owner_exe ? L'x' : L'-')
										% ((tmp_perms  & boost::filesystem::group_read) == boost::filesystem::group_read ? L'r' : L'-')
										% ((tmp_perms & boost::filesystem::group_write) == boost::filesystem::group_write ? L'w' : L'-')
										% ((tmp_perms & boost::filesystem::group_exe) == boost::filesystem::group_exe ? L'x' : L'-')
										% ((tmp_perms  & boost::filesystem::others_read) == boost::filesystem::others_read ? L'r' : L'-')
										% ((tmp_perms & boost::filesystem::others_write) == boost::filesystem::others_write ? L'w' : L'-')
										% ((tmp_perms & boost::filesystem::others_exe) == boost::filesystem::others_exe ? L'x' : L'-')
										% tmp_filesize
										% month_string[t->tm_mon]
										% t->tm_mday % szYearOrHour
										% iter->path().filename().wstring().c_str()
										% ((opt_F && is_directory(tmp_status)) ? L"/" : L""));


									if (send_reply(strBuf.c_str(), is_ctrl) == false)
									{
										break;
									}

								}
							}

						}

					}

				}

			}
			if (cmd.id == commands::STAT)
			{
				send_reply(213, L"End of status.");
			}
			else
			{

				close_data_socket();
				{
					std::lock_guard<std::mutex> lock(ctrl_socket_lock_);
					if (is_ctrl_canal_open_ == true)
					{
						send_reply(226, L"Transfer complete.");

						this->cur_data_mode_ = ftp_data_mode::mode_none;
						this->cur_status_ = ftp_client_status::status_waiting;
					}
				}

			}

		}
		/****************************************
		* TRANSFER
		***************************************/
		bool client_node::process_list_cmd(const t_command& cmd, reply& ftpreply)
		{


			if (!(user_->get_user_privilege() & (unsigned char)user_privilege::list_dir)) {

				ftpreply.create(550,L"Permission denied.");
				return true;

			}
			if (cmd.id != commands::STAT && cur_status_ != ftp_client_status::status_waiting) 
			{

				ftpreply.create(425,L"You're already connected.");
				return true;

			}

			if (cmd.id != commands::STAT) 
			{

				if (this->cur_data_mode_ == ftp_data_mode::mode_none || !open_data_connection())
				{

					ftpreply.create(425,L"Can't open data connection.");
					return true;

				}
			}
			boost::thread t1(boost::bind(&client_node::list_thread, this,cmd));
			if (t1.joinable() == true)
			{
				t1.detach();

			}
			else
			{
				ftpreply.create(425,L"425 Can't open data connection.");
			}

			return true;
		}
		bool client_node::process_cwd_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (cmd_arg.empty() == false)
			{
				boost::filesystem::file_status st;
				std::wstring tmp_path = build_path(user_->get_home_directory(),current_directory_, cmd_arg);

				if (tmp_path.empty() == false && get_file_status(tmp_path, st) == true && is_directory(st) == true)
				{

					int home_dir_len = user_->get_home_directory().length();
					if (tmp_path.length() <= home_dir_len)
					{
						tmp_path = user_->get_home_directory();
						current_directory_ = L"/";
					}
					else
					{
						home_dir_len = ((user_->get_home_directory()[home_dir_len - 1] == L'/') ? home_dir_len - 1 : home_dir_len);
						//wcscpy(Client->szCurrentDir, iStartPathLen +
						//	((Client->User->szStartDir[iStartPathLen - 1] == L'/') ? pszPath - 1 : pszPath));

						current_directory_ = tmp_path.substr(home_dir_len);
					}

					if (current_directory_.empty() == true)
						current_directory_ = L"/";

					ftpreply.create(250,L"CWD command successful.");

				}
				else
				{
					ftpreply.create(550,L"No such file or directory.");
				}


			}
			else
			{
				ftpreply.create(501,L"Invalid number of arguments.");
			}
			return true;
		}
		bool client_node::process_mdtm_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (cmd_arg.empty() == false)
			{
				boost::filesystem::file_status st;
				boost::system::error_code ec;

				std::wstring tmp_path = build_path(user_->get_home_directory(), current_directory_, cmd_arg);

				if (tmp_path.empty() == false && get_file_status(tmp_path, st) == true) 
				{

					time_t last = boost::filesystem::last_write_time(boost::filesystem::path(tmp_path), ec);
					if (!ec)
					{
						struct tm *t = gmtime(&last);
						ftpreply.create(213,(boost::wformat(L"%04d%02d%02d%02d%02d%02d") % (t->tm_year + 1900) % (t->tm_mon + 1) % t->tm_mday % t->tm_hour % t->tm_min % t->tm_sec).str());

					}
					else
					{
						ftpreply.create(550,L"No such file or directory.");
					}
				}
				else
				{
					ftpreply.create(550, L"No such file or directory.");
				}

			}
			else
			{
				ftpreply.create(501, L"Invalid number of arguments.");
			}

			return true;
		}
		bool client_node::process_cdup_cmd(wstring cmd_arg, reply& ftpreply)
		{

			if (current_directory_ != L"/")
			{
				while (current_directory_.empty() == false && *current_directory_.rbegin() == L'/')
				{
					current_directory_.pop_back();
				}

				size_t pos = current_directory_.rfind(L'/');
				if (pos != std::wstring::npos && pos != 0)
				{
					current_directory_ = current_directory_.substr(0, pos);

				}
				else
				{
					current_directory_ = L"/";
				}

			}

			ftpreply.create(250,L"CDUP command successful.");

			return true;
		}
		bool client_node::process_abor_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (cur_status_ != ftp_client_status::status_waiting) 
			{
				close_socket(data_socket_);
				cur_status_ = ftp_client_status::status_waiting;
				cur_data_mode_ = ftp_data_mode::mode_none;
				send_reply(426, L"Previous command has been finished abnormally.");

			}
			ftpreply.create(226,L"ABOR command successful.");

			return true;
		}
		bool client_node::process_rest_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (cmd_arg.empty() && cur_status_ == ftp_client_status::status_waiting) 
			{

#ifdef __USE_FILE_OFFSET64
				current_transfer_.restart_at_ = wcstoll(cmd_arg.c_str(), nullptr, 10);
#else
				current_transfer_.restart_at_ = wcstol(cmd_arg.c_str(), nullptr, 10);
#endif
				ftpreply.create(350,L"REST command successful.");

			}
			else
			{
				ftpreply.create(501,L"Syntax error in arguments.");
			}
			return true;
		}
		bool client_node::process_retr_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (!(user_->get_user_privilege() & (unsigned char)user_privilege::read_file)) 
			{

				ftpreply.create(550,L"Permission denied.");
				return true;

			}
			if (cmd_arg.empty() == false) 
			{

				if (cur_status_ != ftp_client_status::status_waiting) 
				{

					ftpreply.create(425,L"You're already connected.");
					return true;

				}
				if (cur_data_mode_ == ftp_data_mode::mode_none || !open_data_connection()) 
				{

					ftpreply.create(425,L"Can't open data connection.");
					return true;

				}
				boost::filesystem::file_status st;
				std::wstring tmp_path = build_path(user_->get_home_directory(), current_directory_, cmd_arg);
				if (tmp_path.empty() == false && get_file_status(tmp_path, st) == true) 
				{

					cur_status_ = ftp_client_status::status_downloading;
					current_transfer_.transfer_path_ = tmp_path;

					ftpreply.create(150,L"Opening data connection.");

					boost::thread t1(boost::bind(&client_node::retrieve_thread, this));
					if (t1.joinable() == false)
					{

						memset(&current_transfer_, 0x0, sizeof(current_transfer_));
						cur_status_ = ftp_client_status::status_waiting;
						ftpreply.create(425,L"Opening data connection failed.");
					}
					else
					{
						t1.detach();
					}

				}
				else
				{
					ftpreply.create(550,L"File not found.");
				}

			}
			else
			{
				ftpreply.create(501,L"501 Syntax error in arguments.");
			}
			return true;
		}

		bool client_node::process_stor_cmd(const t_command& cmd, reply& ftpreply)
		{

			if (!(user_->get_user_privilege() & (unsigned char)user_privilege::write_file))
			{

				ftpreply.create(550, L"Permission denied.");
				return true;

			}
			if (cmd.args.empty() == false || (cmd.id == commands::STOU)) 
			{

				if (cur_status_ != ftp_client_status::status_waiting)
				{

					ftpreply.create(425, L"You're already connected.");
					return true;

				}
				if (cur_data_mode_ == ftp_data_mode::mode_none || !open_data_connection())
				{

					ftpreply.create(425, L"Can't open data connection.");
					return true;

				}

				boost::filesystem::file_status st;
				std::wstring szTempPath(L"");

				if (cmd.id == commands::STOU) 
				{

					std::wstring szFileUniqueName(L"");
					do
					{

						szTempPath.clear();
						szFileUniqueName = boost::str(boost::wformat(L"file.%i") % (int)((float)rand() * 99999 / RAND_MAX));
						szTempPath = build_path(user_->get_home_directory(), current_directory_, szFileUniqueName);

					} while (get_file_status(szTempPath, st) == true);

				}
				std::wstring pszPath(L"");
				if (szTempPath.empty() == false) 
				{

					pszPath = szTempPath;

				}
				else 
				{

					pszPath = build_path(user_->get_home_directory(), current_directory_, cmd.args, false);

				}

				if (pszPath.empty() == false) 
				{
					cur_status_ = ftp_client_status::status_uploading;

					if (cmd.id == commands::APPE) 
					{

						if (get_file_status(pszPath, st) == true) 
						{
							boost::system::error_code ec;
							current_transfer_.restart_at_ = boost::filesystem::file_size(boost::filesystem::path(pszPath), ec);
							if (ec)
							{
								current_transfer_.restart_at_ = 0;
							}

						}
						else
						{
							current_transfer_.restart_at_ = 0;
						}

					}
					current_transfer_.transfer_path_ = pszPath;

					if (cmd.id == commands::STOU) 
					{

						ftpreply.create(150,(boost::wformat(L"FILE: %s") % szTempPath).str());


					}
					else
					{
						ftpreply.create(150,L"Opening data connection.");
					}

					boost::thread t1(boost::bind(&client_node::store_thread, this));
					if (t1.joinable() == false)
					{
						memset(&current_transfer_, 0x0, sizeof(current_transfer_));
						cur_status_ = ftp_client_status::status_waiting;

						ftpreply.create(425,L"Opening data connection failed.");
					}
					else
					{
						t1.detach();
					}

				}

			}
			else
			{
				ftpreply.create(501, L"501 Syntax error in arguments.");
			}
			return true;
		}
		bool client_node::process_size_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (cmd_arg.empty() == false) 
			{
				std::wstring tmp_path = build_path(user_->get_home_directory(), current_directory_, cmd_arg);
				if (tmp_path.empty() == false) 
				{
					boost::system::error_code ec;

					uintmax_t filesize = boost::filesystem::file_size(boost::filesystem::path(tmp_path), ec);
					if (!ec)
					{
						ftpreply.create(213,(boost::wformat(
#ifdef __USE_FILE_OFFSET64
							L"%llu\r\n"
#else
							L"%li\r\n"
#endif
							) % filesize).str());
					}
					else
					{
						ftpreply.create(550,L"No such file.");
					}
				}
				else
				{
					ftpreply.create(550, L"No such file.");
				}

			}
			else
			{
				ftpreply.create(501,L"Syntax error in arguments.");
			}
			return true;
		}
		bool client_node::process_del_cmd(wstring cmd_arg, reply& ftpreply)
		{

			if (!(user_->get_user_privilege() & (unsigned char)user_privilege::delete_file))
			{

				ftpreply.create(550, L"Permission denied.");
				return true;

			}
			if (cmd_arg.empty() == false) 
			{
				boost::filesystem::file_status st;
				boost::system::error_code ec;

				std::wstring tmp_path = build_path(user_->get_home_directory(), current_directory_, cmd_arg);
				if (tmp_path.empty() == false && get_file_status(tmp_path, st) == true && boost::filesystem::is_regular_file(st) == true) 
				{

					if (boost::filesystem::remove(tmp_path, ec) == true) 
					{

						ftpreply.create(250,L"DELE command successful.");

					}
					else
					{
						ftpreply.create(550, L"Can' t Remove or Access Error.");
					}

				}
				else
				{
					ftpreply.create(550, L"No such file.");
				}

			}
			else
			{
				ftpreply.create(501,L"Syntax error in arguments.");
			}
			return true;
		}
		bool client_node::process_rnfr_cmd(wstring cmd_arg, reply& ftpreply)
		{

			if (!(user_->get_user_privilege() & (unsigned char)user_privilege::delete_file))
			{

				ftpreply.create(550, L"Permission denied.");
				return true;

			}
			if (cmd_arg.empty() == false) 
			{
				boost::filesystem::file_status st;
				std::wstring tmp_path = build_path(user_->get_home_directory(), current_directory_, cmd_arg);
				if (tmp_path.empty() == false && get_file_status(tmp_path, st) == true) 
				{
					rename_from_path_ = tmp_path;

					ftpreply.create(350,L"File or directory exists, ready for destination name.");

				}
				else
				{
					ftpreply.create(550, L"No such file or directory.");
				}

			}
			else
			{
				ftpreply.create(501, L"Syntax error in arguments.");
			}
			return true;
		}
		bool client_node::process_rnto_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (cmd_arg.empty() == false)
			{

				if (rename_from_path_.empty() == false) 
				{
					boost::system::error_code ec;

					std::wstring tmp_path = build_path(user_->get_home_directory(), current_directory_, cmd_arg, false);
					if (tmp_path.empty() == false)
					{
						boost::filesystem::rename(boost::filesystem::path(rename_from_path_), boost::filesystem::path(tmp_path), ec);
						if (!ec)
							ftpreply.create(250,L"Rename successful.");
						else
							ftpreply.create(550,L"Rename failure.");

					}
					else
					{
						ftpreply.create(550, L"Rename failure.");
					}
					rename_from_path_.clear();

				}
				else
				{
					ftpreply.create(503, L"Bad sequence of commands.");
				}

			}
			else
			{
				ftpreply.create(501, L"Syntax error in arguments.");
			}
			return true;
		}
		bool client_node::process_mkd_cmd(wstring cmd_arg, reply& ftpreply)
		{

			if (!(user_->get_user_privilege() & (unsigned char)user_privilege::create_dir))
			{

				ftpreply.create(550, L"Permission denied.");
				return true;

			}
			if (cmd_arg.empty() == false) 
			{
				boost::system::error_code ec;
				boost::filesystem::file_status st;

				std::wstring tmp_path = build_path(user_->get_home_directory(), current_directory_, cmd_arg, false);
				if (tmp_path.empty() == false && get_file_status(tmp_path, st) == false) 
				{

					if (boost::filesystem::create_directory(boost::filesystem::path(tmp_path), ec) == false)
					{
						ftpreply.create(550,L"Internal error creating the directory.");

					}
					else
					{
						ftpreply.create(250,L"Directory created successful.");
					}

				}
				else
				{
					ftpreply.create(550, L"Directory Already Exists.");
				}

			}
			else
			{
				ftpreply.create(501, L"Syntax error in arguments.");
			}
			return true;
		}
		bool client_node::process_rmd_cmd(wstring cmd_arg, reply& ftpreply)
		{
			if (!(user_->get_user_privilege() & (unsigned char)user_privilege::delete_dir))
			{
				ftpreply.create(550, L"Permission denied.");
				return true;
			}
			if (cmd_arg.empty() == false)
			{
				boost::system::error_code ec;
				boost::filesystem::file_status st;

				std::wstring tmp_path = build_path(user_->get_home_directory(), current_directory_, cmd_arg, false);
				if (tmp_path.empty() == false && get_file_status(tmp_path, st) == true && is_directory(st) == true)
				{

					if (boost::filesystem::remove(boost::filesystem::path(tmp_path), ec) == false)
					{
						ftpreply.create(450, L"Internal error deleting the directory.");

					}
					else
					{
						ftpreply.create(250, L"Directory deleted successfully.");
					}

				}
				else
				{
					ftpreply.create(550, L"Directory not found.");
				}

			}
			else
			{
				ftpreply.create(501, L"Syntax error in arguments.");
			}
			return true;
		}
		void client_node::store_thread()
		{

			//	__int64 DataRecv = 0;
			std::streamsize len = -1;

			std::ios_base::openmode mode = std::ios_base::out | std::ios_base::binary;

			if (current_transfer_.restart_at_ > 0)
			{

				mode |= std::ios_base::app; //a|b

			}
			else
			{
				mode |= std::ios_base::trunc; //w|b
			}

			boost::filesystem::ofstream ofs;
				
			ofs.open(current_transfer_.transfer_path_, mode);

			if (ofs.is_open() == true)
			{

				if (current_transfer_.restart_at_ > 0)
				{
					ofs.seekp(current_transfer_.restart_at_);
				}
				if (ofs.good() == true)
				{
					boost::system::error_code ec;
					char *pBuffer = new char[CFTPSERVER_TRANSFER_BUFFER_SIZE];

					while (data_socket_->is_open() == true && ofs.good() == true) 
					{
						len = data_socket_->read_some(boost::asio::buffer(pBuffer, CFTPSERVER_TRANSFER_BUFFER_SIZE), ec);
						if (len > 0) 
						{
							ofs.write(pBuffer, len);

						}
						else
							break;

					}
					delete[] pBuffer;

				}
				if (ofs.bad() == true)
				{

					len = -1;
				}
				ofs.close();
			}
			else
			{

				len = -1;
			}

			close_data_socket();
			{
				std::lock_guard<std::mutex> lock(ctrl_socket_lock_);
				if (is_ctrl_canal_open_ == true)
				{
					if (len == 0)
					{
						send_reply(226, L"Transfer complete.");
					}
					else
					{
						send_reply(550, L"Can 't receive file.");
					}


					memset(&current_transfer_, 0x0, sizeof(current_transfer_));
					cur_data_mode_ = ftp_data_mode::mode_none;
					this->cur_status_ = ftp_client_status::status_waiting;
				}
			}

		}

		void client_node::retrieve_thread()
		{

			std::streamsize BlockSize = 0; std::streamsize len = -1;

			boost::filesystem::ifstream in(current_transfer_.transfer_path_, std::ios::in | std::ios::binary);

			if (in.is_open() == true) 
			{

				if (current_transfer_.restart_at_ > 0)
				{
					in.seekg(current_transfer_.restart_at_);
				}
				if (in.good())
				{
					boost::system::error_code ec;
					char *pBuffer = new char[CFTPSERVER_TRANSFER_BUFFER_SIZE];

					while (data_socket_->is_open() == true
						&& in.good() == true)
					{
						in.read(pBuffer, CFTPSERVER_TRANSFER_BUFFER_SIZE);
						BlockSize = in.gcount();
						len = boost::asio::write(*data_socket_, boost::asio::buffer(pBuffer, (size_t)BlockSize), ec);
						if (len <= 0)
							break;

					}
					delete[] pBuffer;

				}
				if (in.bad() == true)
				{
					len = -1;
				}
				in.close();
			}
			else
			{
				len = -1;
			}

			close_data_socket();

			{
				std::lock_guard<std::mutex> lock(ctrl_socket_lock_);
				if (is_ctrl_canal_open_ == true)
				{
					if (len >= 0)
					{

						send_reply(226, L"Transfer complete.");

					}
					else
					{
						send_reply(550, L"550 Can 't Send File.");
					}


					memset(&current_transfer_, 0x0, sizeof(current_transfer_));
					cur_data_mode_ = ftp_data_mode::mode_none;
					this->cur_status_ = ftp_client_status::status_waiting;
				}
			}

		}
	}
}