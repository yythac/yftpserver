/*
* main.cpp
* ~~~~~~~~
* Mail::yyt_hac@163.com
*
*Copyright(c) 2015 yyt_hac
*/

#include "stdafx.h"

#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>
#include <boost/asio.hpp>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.h"
#include "ftp_server.h"
#include "ftpserver_native.h"


#ifdef SERVER_APP
#include <boost/asio/ssl.hpp>
#if defined(_WIN32) 
#pragma comment(lib,"libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#endif

class FtpServerApp : public YCOMMON::YSERVER::ycommon_server_app
{

protected:

	virtual int init(const std::vector<std::string>& args) override
	{

		this->set_default_server_addr(21);
		this->set_default_is_use_ssl(false);
		this->set_default_is_use_raw_data(true);
		//不缓存日志，直接写入文件
//		YLOG_SET_ALWAYS_FLUSH(true); 

		return 0;
	}
	virtual int main(const std::vector<std::string>& args) override
	{
		YINFO_OUT("Begin to init Ftp Server.......");
		
		int ret = 0;

		string tmp_usr_base = "ftp.server.user";
		char tmp_str[10] = { 0 };
		int user_index = 0;
		bool have_user = false;

		vector<string> vStr;
		char* port_range = nullptr;
		char* ftp_user = nullptr;

		char *root_dir = this->get_string("ftp.server.root_dir", "","ftp server");
		char *allow_anonymous = this->get_string("ftp.server.allow_anonymous", "1::rwd", "ftp server");
		YINFO_OUT("Ftp Server Root Dir:%s", root_dir);
		YINFO_OUT("Anonymous User Info:%s", allow_anonymous);
		if (allow_anonymous[0] != 0)
		{
			vStr.clear();
			boost::split(vStr, allow_anonymous, boost::is_any_of(":;,"), boost::token_compress_off);
			if (vStr.size() != 3)
			{
				YFATAL_OUT("Ftp Server Anonymous User Info Invalid!!!");
				ret = -1;
				goto end;
			}
			string user_dir = root_dir;
			if (user_dir.empty() == false && (user_dir.back() != '\\'
				&& user_dir.back() != '/'))
			{
				user_dir.push_back(path::preferred_separator);
			}
			user_dir += vStr[1];
			unsigned char priv = 0;
			for (auto c : vStr[2])
			{
				if (c == 'r')
					priv = priv | (unsigned char)FTP_USER_PRIV_READ;
				else if (c == 'w')
					priv = priv | (unsigned char)FTP_USER_PRIV_WRITE;
				else if (c == 'd')
					priv = priv | (unsigned char)FTP_USER_PRIV_DEL;
			}
			if (ftp_server_.allow_anonymous(vStr[0] == "1", s2ws(user_dir).c_str(), priv) == false)
			{
				YFATAL_OUT("Ftp Server Allow Anonymous Error:%s!!!", ws2s(ftp_server_.get_last_error_string()).c_str());
				ret = -1;
				goto end;
			}
		}

		do 
		{
			snprintf(tmp_str, sizeof(tmp_str), "%d", ++user_index);
			string tmp_usr_info = tmp_usr_base + tmp_str;
			ftp_user = this->get_string(tmp_usr_info, "", "ftp server"); // = "tester1:123456:IconLib:rwd";
			if (ftp_user[0] != 0)
			{
				YINFO_OUT("Ftp Server User Infomation:%s", ftp_user);

				vStr.clear();
				boost::split(vStr, ftp_user, boost::is_any_of(":;,"), boost::token_compress_off);
				if (vStr.size() != 4)
				{
					YERROR_OUT("Ftp Server User Info Invalid!!!");
					continue;
				}
				unsigned char priv = 0;
				for (auto c : vStr[3])
				{
					if (c == 'r')
						priv = priv | (unsigned char)FTP_USER_PRIV_READ;
					else if (c == 'w')
						priv = priv | (unsigned char)FTP_USER_PRIV_WRITE;
					else if (c == 'd')
						priv = priv | (unsigned char)FTP_USER_PRIV_DEL;
				}

				string user_dir = root_dir;
				if (user_dir.empty() == false && user_dir.back() != '\\'
					&& user_dir.back() != '/')
				{
					user_dir.push_back(path::preferred_separator);
				}
				user_dir += vStr[2];
				if (ftp_server_.add_user(s2ws(vStr[0]).c_str(), s2ws(vStr[1]).c_str(), s2ws(user_dir).c_str(), priv) == false)
				{
					YERROR_OUT("Ftp Server Add User Error:%s!!!", ws2s(ftp_server_.get_last_error_string()).c_str());
				}
				YCOMMON::GLOBAL::ycommon_free(ftp_user);

				have_user = true;
			}
			else
			{
				YCOMMON::GLOBAL::ycommon_free(ftp_user);
				break;
			}
			
		} while (true);



		if (allow_anonymous[0] == 0 && have_user == false)
		{
			YFATAL_OUT("Ftp Server No User Infomation!!!");
			ret = -1;
			goto end;
		}
		port_range = this->get_string("ftp.server.pasv_port_range", "0-0", "ftp server");
		if (port_range[0] != 0 && strcmp(port_range ,"0-0") != 0)
		{
			YINFO_OUT("Ftp Server pasv port range:%s", port_range);

			vStr.clear();
			boost::split(vStr, port_range, boost::is_any_of("-: "), boost::token_compress_on);
			if (vStr.size() != 2)
			{
				YERROR_OUT("ftp.server.pasv_port_range is Invalid!");
			}
			else
			{
				unsigned short start_port = atoi(vStr[0].c_str());
				unsigned short end_port = atoi(vStr[1].c_str());
				if (ftp_server_.set_data_port_range(start_port, end_port - start_port + 1) == false)
				{
					YERROR_OUT("Set data port range error!");
				}
			}
		}
		ftp_server_.ssl_context(this->boost_ssl_context());
end:

		YCOMMON::GLOBAL::ycommon_free(root_dir);
		YCOMMON::GLOBAL::ycommon_free(port_range);
		YCOMMON::GLOBAL::ycommon_free(allow_anonymous);

		return ret;
	}
	/*
	//返回服务器私钥密码,默认是从配置文件中获取，所以如果不想改变默认方式就不用重载该函数
	virtual std::string& get_key_file_password() const override
	{

		static std::string pass("");
		char* key_pass = get_string("server.key_file_password", "", "common server");
		pass = key_pass;
		YCOMMON::GLOBAL::ycommon_free(key_pass);
		return pass;
		
	}*/
	virtual void on_connect(YCOMMON::YSERVER::i_ycommon_socket* conn) override
	{
//		YINFO_OUT( L"Connection:%X connected", conn);
		ftp::server::reply ftpreply;

		ftp_server_.start_work(ftpreply);
		if (ftpreply.empty() == false)
		{
			conn->i_send_data(ftpreply.to_string().data(), ftpreply.to_string().size());
		}
		return ;
	}
	//数据处理回调函数,重载该函数用于处理数据包
	//客户端发送的包为（2字节长度+数据内容）
	//这里收到的数据为 数据内容，已经自动根据数据包长度收到完整的数据，2字节长度已去除
	virtual bool on_process_data(YCOMMON::YSERVER::i_ycommon_socket* conn, const char* pdata, int len) override
	{
		//YINFO_OUT("Connection:%X recv data,len:%d", conn,len);

		ftp::server::reply ftpreply;
		bool result;

		result = ftp_server_.process_command(*conn,
			(char*)pdata, len, ftpreply);
		if (ftpreply.empty() == false)
		{
			conn->i_send_data(ftpreply.to_string().data(), ftpreply.to_string().size());
		}
		if (result == false)
		{
			ftp_server_.end_work(*conn);
			conn->i_shutdown();
			conn->i_disconnect();
		}
		return true;

	}
	//连接关闭回调函数，重载该函数用于连接关闭后的清理工作
	virtual void on_close(YCOMMON::YSERVER::i_ycommon_socket* conn) override
	{
//		YINFO_OUT("Connection:%X disconnect", conn);

		ftp_server_.end_work(*conn);

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