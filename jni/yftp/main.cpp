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
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.hpp"
#include "ftp_server.h"
#include "ftpserver_native.h"


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
