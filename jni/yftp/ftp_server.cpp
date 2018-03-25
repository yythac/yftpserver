/*
* user_manager.h :: ftp user manager class header file
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/
/*
* ftp_server.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#include "stdafx.h"
#include <locale>
#include <codecvt>

#include "conversion.h"
#include "ftp_server.h"


namespace ftp {
	namespace server {

		/****************************************
		 * CONSTRUCTOR && DESTRUCTOR
		 ***************************************/

		data_port_range ftp_server::data_port_range_ = { 0 };

		ftp_server::ftp_server(void)
		{

			int i = 0;

//			this->data_port_range_.start = 0;
//			this->data_port_range_.num = 0;

			this->is_allow_anonymous_ = false;

			srand((unsigned)time(nullptr));

		}

		ftp_server::~ftp_server(void)
		{

			user_manager_.delete_all_user();

		}
		//添加用户
		//参数：
		//name：需要添加的用户名
		//password:用户密码
		//home_dir:用户起始目录
		//priv:用户权限
		//返回：	
		//true:添加用户成功
		//false:添加用户失败
		bool ftp_server::add_user(const wstring& name, const wstring& password
			, const wstring& home_dir, unsigned char priv)
		{
			last_error_.set_error_code(0);

			if (name == L"anonymous" || name == L"ftp")
			{
				last_error_.set_error_code(YFTP_ERROR_USERNAME_INVALID);
				return false;
			}
			if (user_manager_.add_user(name, password, home_dir, priv) == false)
			{
				last_error_.set_error_code(user_manager_.get_last_error());
				return false;
			}
			else
			{
				return true;
			}
		}
		/****************************************
		 * CONFIG
		 ***************************************/

		bool ftp_server::allow_anonymous(bool do_allow, const std::wstring& home_dir, unsigned char priv)
		{
			last_error_.set_error_code(0);

			if (do_allow == true)
			{

				if (this->is_allow_anonymous_ == false)
				{

					if (user_manager_.add_user(L"anonymous", L"", home_dir, priv) == false)
					{
						last_error_.set_error_code(user_manager_.get_last_error());
						return false;
					}
					this->is_allow_anonymous_ = true;
				}

				return true;

			}
			else
			{

				if (this->is_allow_anonymous_ == true)
				{

					if (user_manager_.delete_user(L"anonymous") == false
						&& user_manager_.get_last_error() != YFTP_ERROR_USER_NOT_FOUND)
					{
						last_error_.set_error_code(user_manager_.get_last_error());
						return false;
					}

					this->is_allow_anonymous_ = false;

				}
				return true;

			}

		}


		bool ftp_server::start_work(reply& ftpreply)
		{
			ftpreply.create(220, L"Welcome to yyt_hac's ftp server.");
			return true;

		}
		bool ftp_server::end_work(boost::asio::ip::tcp::socket& ctrl_socket)
		{
			sp_client_node client = user_manager_.find_client(ctrl_socket);
			if (client != nullptr)
			{
				client->end();
			}
			return true;
		}
		bool ftp_server::process_command(boost::asio::ip::tcp::socket& ctrl_socket, char *pdata, int datalen, reply& ftpreply)
		{
			bool ret = true;

			boost::system::error_code ec;
			t_command cmd;

			sp_client_node client = user_manager_.find_client(ctrl_socket);
			if (client == nullptr)
			{
				client = make_shared<client_node>();
				if (client == nullptr)
				{
					return false;
				}
				client->start(ctrl_socket);

			}

			ftpreply.create(0, L"");
			ftpreply.set_code_type(client->get_code_type());

			boost::tribool tribresult;

			client->get_parser().set_status(client->get_is_logged());
			tribresult = client->get_parser().parse(cmd, pdata, datalen, ftpreply);
			if (tribresult != true)
			{
				return true;
			}

			//YDEBUG_OUT("ftp server recv:%s", pdata);

			switch (cmd.id)
			{
				// Enumerate the Commands
			case commands::QUIT:
			{
				ftpreply.create(221, L"Goodbye.");
				break;
			}
			case commands::USER:
			{

				ret = client->process_user_cmd(is_allow_anonymous_, user_manager_, cmd.args, ftpreply);
				break;


			}
			case commands::PASS:
			{
				ret = client->process_password_cmd(cmd.args, ftpreply);
				break;

			}
			case commands::NOOP:
			case commands::ALLO:
			{

				ftpreply.create(200, L"NOOP Command Successful.");
				break;
			}

#ifdef CFTPSERVER_USE_EXTRACMD

			case commands::SITE:
			{

				ret = client->process_site_cmd(cmd.args, ftpreply);
				break;

			}
#endif
			case commands::HELP:
			{

				ftpreply.create(500, L"No Help Available.");
				break;

			}
			case commands::REIN:
			{
				ret = client->process_rein_cmd(cmd.args, ftpreply);

				break;
			}
			case commands::SYST:
			{

				ftpreply.create(215, L"UNIX Type: L8");
				break;

			}
			case commands::STRU:
			{

				if (cmd.args.empty() == false)
				{

					if (boost::to_upper_copy(cmd.args) == L"F")
					{

						ftpreply.create(200, L"STRU Command Successful.");

					}
					else
					{
						ftpreply.create(504, L"STRU failled. Parametre not implemented.");
					}

				}
				else
				{
					ftpreply.create(501, L"Invalid number of arguments.");
				}
				break;


			}
			case commands::MODE:
			{

				if (cmd.args.empty() == false)
				{

					boost::to_upper(cmd.args);
					if (cmd.args == L"S")
					{

						ftpreply.create(200, L"Mode set to S.");

					}
					else if (cmd.args == L"C")
					{
						// here zlib compression
						ftpreply.create(502, L"MODE non-implemented.");

					}
					else
					{
						ftpreply.create(504, (boost::wformat(L"\"%s\": Unsupported transfer MODE.") % cmd.args).str());

					}

				}
				else
				{
					ftpreply.create(501, L"Invalid number of arguments.");
				}
				break;


			}
			case commands::TYPE:
			{
				ret = client->process_type_cmd(cmd.args, ftpreply);
				break;

			}
			case commands::PORT:
			{

				ret = client->process_port_cmd(cmd.args, ftpreply);
				break;


			}
			case commands::PASV:
			{

				ret = client->process_pasv_cmd(cmd.args, ftpreply);
				break;

			}
			case commands::LIST:
			case commands::NLST:
			case commands::MLSD:
			{
				ret = client->process_list_cmd( cmd, ftpreply);
				break;

			}
			case commands::CWD:
			{
				ret = client->process_cwd_cmd(cmd.args, ftpreply);
				break;
			}
			case commands::FEAT:
			{
				ftpreply.create(0, L"211-Features:\r\n"
					L" MDTM\r\n"
					L" SIZE\r\n"
					L" UTF8\r\n"
					L"211 End\r\n");

				break;

			}
			case commands::MDTM:
			{
				ret = client->process_mdtm_cmd(cmd.args, ftpreply);
				break;
			}
			case commands::PWD:
			{
				ftpreply.create(257, (boost::wformat(L"\"%s\" is current directory.") % client->get_current_directory()).str());
				break;
			}
			case commands::CDUP:
			{
				ret = client->process_cdup_cmd(cmd.args, ftpreply);
				break;

			}
			case commands::STAT:
			{

				if (cmd.args.empty() == false)
				{
					ret = client->process_list_cmd(cmd, ftpreply);
				}
				else
				{
					ftpreply.create(211, L":: ftp_server / Browser FTP Server:: yyt_hac@163.com");
				}

				break;


			}
			case commands::ABOR:
			{
				ret = client->process_abor_cmd(cmd.args, ftpreply);
				break;

			}
			case commands::REST:
			{
				ret = client->process_rest_cmd(cmd.args, ftpreply);
				break;
			}
			case commands::RETR:
			{
				ret = client->process_retr_cmd(cmd.args, ftpreply);
				break;
			}
			case commands::STOR:
			case commands::APPE:
			case commands::STOU:
			{

				ret = client->process_stor_cmd(cmd, ftpreply);
				break;
			}
			case commands::SIZE:
			{
				ret = client->process_size_cmd(cmd.args, ftpreply);
				break;
			}
			case commands::DELE:
			{
				ret = client->process_del_cmd(cmd.args, ftpreply);
				break;
			}
			case commands::RNFR:
			{
				ret = client->process_rnfr_cmd(cmd.args, ftpreply);
				break;
			}
			case commands::RNTO:
			{
				ret = client->process_rnto_cmd(cmd.args, ftpreply);
				break;
			}
			case commands::MKD:
			{
				ret = client->process_mkd_cmd(cmd.args, ftpreply);
				break;

			}
			case commands::RMD:
			{
				ret = client->process_rmd_cmd(cmd.args, ftpreply);
				break;

			}
			case commands::OPTS:
			{
				if (cmd.args == L"UTF8 ON" || cmd.args == L"UTF-8 ON")
					ftpreply.create(202, L"UTF8 mode is always enabled. No need to send this command.");
				else if (cmd.args == L"UTF8 OFF" || cmd.args == L"UTF-8 OFF")
					ftpreply.create(504, L"UTF8 mode cannot be disabled.");
				else
					ftpreply.create(501, L"Option not understood");
				break;
			}
			default:
			{

				ftpreply.create(500, L"Command not understood.");
				break;

			}

			}


			return ret;
		}

		bool ftp_server::delete_user(const wstring& name)
		{
			return false;
	/*		if (User != nullptr) 
			{


				User->bIsEnabled = false;

				{
					std::lock_guard<std::mutex> lock(m_csClientRes);
					if (User->iNbClient > 0) {

						struct ClientNode *Client = this->ClientListHead;
						struct ClientNode *NextClient = nullptr;
						while (Client && User->iNbClient > 0) {

							NextClient = Client->NextClient;
							if (Client->bIsLogged && Client->User == User) {

								if (Client->CtrlSock != INVALID_SOCKET) {

									this->CloseSocket(Client->CtrlSock);
									Client->CtrlSock = INVALID_SOCKET;

								}
								if (Client->DataSock != INVALID_SOCKET) {

									this->CloseSocket(Client->DataSock);
									Client->DataSock = INVALID_SOCKET;

								}
								// the Client Thread should now end up.

							}
							Client = NextClient;

						}

					}

				}

				while (User->iNbClient > 0)
				{

					boost::this_thread::sleep_for(boost::chrono::milliseconds(10));

				}

				{
					std::lock_guard<std::mutex> lock(m_csUserRes);
					if (User->PrevUser) User->PrevUser->NextUser = User->NextUser;
					if (User->NextUser) User->NextUser->PrevUser = User->PrevUser;
					if (this->UserListHead == User) this->UserListHead = User->NextUser;
					if (this->UserListLast == User) this->UserListLast = User->PrevUser;
				}


				delete User;
				this->iNbUser += -1;
				return true;

			}
			return false;
			*/
		}

	}
}
