
#include "stdafx.h"
#include "server.h"


int g_listen_port = 0;
std::wstring g_start_dir(L"");
unsigned char g_ucPriv;
bool g_bAllowAnonymous = false;
std::wstring g_username(L"");
std::wstring g_password(L"");

ftp::server::YFtpServer ftp_server;


bool init_ftp_server(bool bDoAllow,const wchar_t *szusername,const wchar_t *szpassword,
		const wchar_t *szStartPath, unsigned char ucPriv,int port)
{
//	LOGI("entring init_ftp_server...");
	bool bret = true;
	if(g_bAllowAnonymous != bDoAllow)
	{
		bret = ftp_server.allow_anonymous(bDoAllow,szStartPath,ucPriv);
		if(bret == true)
		{
			g_bAllowAnonymous = bDoAllow;
			g_ucPriv = ucPriv;
		}
	}
	if(g_listen_port != port)
	{
		ftp_server.SetListeningPort(port);
		g_listen_port = port;
	}

	if(g_username != szusername)
	{
		if(g_username.empty() == false)
		{
			ftp_server.delete_user(g_username.c_str());
			g_username.clear();
		}
		if(szusername[0] != L'\0')
		{
			if(ftp_server.add_user(szusername,szpassword,szStartPath, ucPriv) != false)
			{
				g_username = szusername;
				g_password = szpassword;
				g_ucPriv = ucPriv;
			}
		}
	}
	else if(g_password != szpassword)
	{
		if(ftp_server.set_user_password(szusername,szpassword) == true)
		{
			g_password = szpassword;
		}
	}
	if(g_start_dir != szStartPath)
	{
		if(ftp_server.set_user_home_directory(L"",szStartPath) == true)
			g_start_dir = szStartPath;
	}

	if(g_ucPriv != ucPriv)
	{
		if(ftp_server.set_user_privilege(L"",ucPriv) == true)
			g_ucPriv = ucPriv;
	}

	return bret;
}

bool start_ftp_server()
{
	bool bret = true;
	bret = ftp_server.StartListening();
	if(bret == true)
	{
		boost::thread myrun(boost::bind(&ftp::server::YFtpServer::run,&ftp_server));
		if(myrun.joinable() == true)
		{
			myrun.detach();
		}
		else
		{
			bret = false;
		}
	}

	return bret;
}
bool stop_ftp_server()
{
	bool bret = true;
	bret = ftp_server.StopListening();
	if (bret == true)
	{
		ftp_server.stop();
		int i = 0;
		while (ftp_server.IsStoped() == false && i++ < 10)
		{
			boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
		}

		bret = ftp_server.IsStoped();
	}

	return bret;
}
