/*
* main.cpp
* ~~~~~~~~
* Mail::yyt_hac@163.com
*
*Copyright(c) 2015 yyt_hac
*/

#include "stdafx.h"

#include <iostream>
#include <string>
#include <map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.hpp"
#include "ftp_server.h"
#include "ftpserver_native.h"

#define SERVER_APP

#ifdef SERVER_APP

#include "ycommon_server_app.h"

class FtpServerApp : public YCOMMON::YSERVER::ycommon_server_app
{

protected:

	virtual int main(const std::vector<std::string>& args) override
	{
		//cout << "Begin to init Y-FTP server.......";

		char *root_dir = this->get_string("ftp.server.root_dir", ""); 
		bool allow_anonymous = !(this->get_int("ftp.server.allow_anonymous", 1) == 0);
		YINFO_OUT("Ftp Server Root Dir:%s,Is Allow Anonymous:%s", root_dir, allow_anonymous ? "true" : "false");
		if (allow_anonymous)
		{
			if (ftp_server_.allow_anonymous(allow_anonymous, s2ws(root_dir).c_str(), FTP_USER_PRIV_ALL) == false)
			{
				YCOMMON::GLOBAL::ycommon_free(root_dir);
				YFATAL_OUT("Ftp Server Allow Anonymous Error!!!");
				return -1;
			}
		}

		char* ftp_user = this->get_string("ftp.server.user1", ""); // = "tester1:123456:IconLib:rwd";
		if (ftp_user[0] != 0)
		{
			YINFO_OUT("FTP Server User Infomation:%s", ftp_user);
			vector<string> vStr;
			vStr.clear();
			boost::split(vStr, ftp_user, boost::is_any_of(":;,"), boost::token_compress_off);
			if (vStr.size() != 4)
			{
				YCOMMON::GLOBAL::ycommon_free(root_dir);
				YCOMMON::GLOBAL::ycommon_free(ftp_user);
				YFATAL_OUT("Ftp Server User Info Invalid!!!");
				return -1;
			}
			unsigned char priv = 0;
			for (auto c : vStr[3])
			{
				if (c == 'r')
					priv = priv | (unsigned char)FTP_USER_PRIV_READ;
				else if(c == 'w')
					priv = priv | (unsigned char)FTP_USER_PRIV_WRITE;
				else if(c == 'd')
					priv = priv | (unsigned char)FTP_USER_PRIV_DEL;
			}
		
			string user_dir = root_dir; 
			if (user_dir.back() != '\\'
				&& user_dir.back() != '/')
			{
				user_dir.push_back(path::preferred_separator);
			}
			user_dir += vStr[2];
			if (ftp_server_.add_user(s2ws(vStr[0]).c_str(), s2ws(vStr[1]).c_str(), s2ws(user_dir).c_str(), priv) == false)
			{
				YCOMMON::GLOBAL::ycommon_free(root_dir);
				YCOMMON::GLOBAL::ycommon_free(ftp_user);
				YFATAL_OUT("Ftp Server Add User Error!!!");
				return -1;
			}

		}
		YCOMMON::GLOBAL::ycommon_free(root_dir);
		YCOMMON::GLOBAL::ycommon_free(ftp_user);

		return 0;
	}
	//返回服务器私钥密码
	virtual std::string& get_key_file_password() const override
	{
		static std::string pass; //不能用临时变量，必须用静态，或类成员变量
		pass = "test";
		return pass;
	}
	virtual void on_connect(void* conn) override
	{
//		YINFO_OUT( L"Connection:%X connected", conn);
		ftp::server::reply ftpreply;

		ftp_server_.start_work(ftpreply);
		if (ftpreply.empty() == false)
		{
			this->send_data(conn, ftpreply.to_string().data(), ftpreply.to_string().size());
		}
		return ;
	}
	//数据处理回调函数,重载该函数用于处理数据包
	//客户端发送的包为（2字节长度+数据内容）
	//这里收到的数据为 数据内容，已经自动根据数据包长度收到完整的数据，2字节长度已去除
	virtual bool on_process_data(void* conn, const char* pdata, int len) override
	{
		//		YINFO_OUT("Connection:%X recv data,len:%d", conn,len);

		ftp::server::reply ftpreply;
		bool result;

		result = ftp_server_.process_command((boost::asio::ip::tcp::socket&)*((boost::asio::ip::tcp::socket*)boost_handle(conn)),
			(char*)pdata, len, ftpreply);
		if (ftpreply.empty() == false)
		{
			this->send_data(conn, ftpreply.to_string().data(), ftpreply.to_string().size());
		}
		if (result == false)
		{
			ftp_server_.end_work((boost::asio::ip::tcp::socket&)*((boost::asio::ip::tcp::socket*)boost_handle(conn)));
			shutdown(conn);
			disconnect(conn);
		}
		return true;
		//		shutdown(conn);
		//		disconnect(conn);
	}
	//连接关闭回调函数，重载该函数用于连接关闭后的清理工作
	virtual void on_close(void* conn) override
	{
//		YINFO_OUT("Connection:%X disconnect", conn);
		ftp_server_.end_work((boost::asio::ip::tcp::socket&)*((boost::asio::ip::tcp::socket*)boost_handle(conn)));

	}
private:

	ftp::server::ftp_server ftp_server_;
};

YCOMMON_SERVER_MAIN(FtpServerApp)
#else

void usage(char *app)
{
	const char *tmp_str = strrchr(app, L'\\');
	if (tmp_str != nullptr)
	{
		tmp_str++;
	}
	else
	{
		tmp_str = app;
	}
	cout << "Usage: " << tmp_str << " <listen port>  <root directory>" << endl;
	cout << "example: " << tmp_str <<" 21 ." << endl;
}

int main(int argc, char* argv [])

{
	try
	{

		int port = -1;
		std::string root_dir("");

		try
		{
			if (argc == 1)
			{
				port = 21;
				root_dir = ".";
			}
			else if (argc == 2)
			{
				port = boost::lexical_cast<int>(argv[1]);
				root_dir = ".";
			}
			else if (argc == 3)
			{
				port = boost::lexical_cast<int>(argv[1]);
				root_dir = argv[2];
			}
		}
		catch (...)
		{

		}
		if (port <= 0)
		{
			usage(argv[0]);
			return -1;
		}
		cout << "Begin to init Y-FTP server.......";
		if(init_ftp_server(true, L"", L"", s2ws(root_dir).c_str(), FTP_USER_PRIV_ALL, port) == false)
		{
			cout << "failed!" << endl;
			return -1;
		}
		cout << "successfully!" << endl;

		cout << "Begin to start Y-FTP server.......";
		if (start_ftp_server() == false)
		{
			cout << "failed!" << endl;
			return -1;
		}
		cout << "successfully!" << endl;
		cout << "Y-FTP server is listening on port : " << port << ", root directory : " << root_dir << endl;
		cout << "Press 'q'+ Enter to exit......" << endl;

		while (true)
		{
			if (getchar() == 'q')
				break;
		}

		stop_ftp_server();
		
		return 1;

	}
	catch (std::exception& e)
	{
		cerr << "exception: " << e.what() << endl;
	}

	return 0;
}
#endif