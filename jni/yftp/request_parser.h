/*
* request_parser.hpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#pragma once

#include <list>
#include <map>
#include <string>
#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include "reply.h"


namespace ftp {
namespace server {

	enum class commands {
		invalid = -1,
		USER,
		PASS,
		QUIT,
		CWD,
		PWD,
		PORT,
		PASV,
		TYPE,
		LIST,
		REST,
		CDUP,
		RETR,
		STOR,
		SIZE,
		DELE,
		RMD,
		MKD,
		RNFR,
		RNTO,
		ABOR,
		SYST,
		NOOP,
		APPE,
		NLST,
		MDTM,
		NOP,
		EPSV,
		EPRT,
		AUTH,
		ADAT,
		PBSZ,
		PROT,
		FEAT,
		MODE,
		OPTS,
		HELP,
		ALLO,
		MLST,
		MLSD,
		SITE,
		PASVSMC, // some bugged SMC routers convert incoming PASV into P@SW
		STRU,
		CLNT,
		MFMT,
		HASH,
		REIN,
		STAT,
		STOU
	};

	struct t_command
	{
		commands id;
		std::wstring args;
		bool bHasargs;
		bool bValidBeforeLogon;
	};

/// Parser for incoming requests.
class request_parser
{
public:
  /// Construct ready to parse the request method.
  request_parser();

  /// Reset to initial parser state.
  void reset();

  /// Parse some data. The tribool return value is true when a complete request
  /// has been parsed, false if the data is invalid, indeterminate when more
  /// data is required.
  boost::tribool parse(t_command& cmd,char *pdata, int datalen, reply& result);
  void set_status(bool blogon)
  {
	  m_status.loggedon = blogon;
  }

private:

	std::list<std::string> m_RecvLineBuffer;
	char m_RecvBuffer[2048];
	int m_nRecvBufferPos{};
	int m_nTelnetSkip{};
	time_t m_LastCmdTime, m_LastTransferTime, m_LoginTime;

	struct t_status
	{
		bool loggedon{false};
		int hammerValue{0};
	} m_status;

	t_command GetCommand(reply& result);
	t_command MapCommand(std::wstring const& command, std::wstring const& args, reply& result);

};

} // namespace server
} // namespace ftp


