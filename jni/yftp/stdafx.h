#pragma once


// Compilation Options ! :
// 98% of the time, shouldn't be modified

// Uncomment the next line if you want to allow Clients to run extra commands
//#define CFTPSERVER_USE_EXTRACMD

// Uncomment the next line if you want to use bruteforcing protection
//#define CFTPSERVER_ANTIBRUTEFORCING

// Uncomment the next line if you want ftp_server to check if the IP supplied with
// the PORT command and the Client IP are the same. Strict IP check may fail when
// the server is behind a router.
//#define CFTPSERVER_STRICT_PORT_IP

// Here is the switch to enable large file support under Win. ( gcc and MS.VC++ )

//#define __USE_FILE_OFFSET64

#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

//#define SERVER_APP

#ifdef SERVER_APP

#include "ycommon_server_app.h"
#else

#define YFATAL_OUT(...)		;
#define YERROR_OUT(...)		;
#define YWARNING_OUT(...)	;
#define YINFO_OUT(...)		;
#define YDEBUG_OUT(...)	

#endif