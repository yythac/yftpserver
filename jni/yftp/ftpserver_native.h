#ifndef FTPSERVER_NATIVE_H
#define FTPSERVER_NATIVE_H

#include "server.h"
#include "ftp_server.h"

bool init_ftp_server(bool bDoAllow, const wchar_t *szusername,const wchar_t *szpassword
		,const wchar_t *szStartPath, unsigned char ucPriv=FTP_USER_PRIV_READ,int port=2121);
bool start_ftp_server();
bool stop_ftp_server();

#endif
