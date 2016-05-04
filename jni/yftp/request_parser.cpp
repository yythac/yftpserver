/*
* request_parser.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/
#include "stdafx.h"
#include <time.h>
#include "request_parser.hpp"
#include "conversion.h"

namespace ftp {
namespace server {

request_parser::request_parser()
{
}

void request_parser::reset()
{

}
std::map<std::wstring, t_command> const command_map = {
	{ L"USER",{ commands::USER,L"", true,  true } },
	{ L"PASS",{ commands::PASS,L"", false, true } },
	{ L"QUIT",{ commands::QUIT,L"", false, true } },
	{ L"CWD",{ commands::CWD,L"",  false, false } },
	{ L"PWD",{ commands::PWD,L"",  false, false } },
	{ L"PORT",{ commands::PORT,L"", true,  false } },
	{ L"PASV",{ commands::PASV,L"", false, false } },
	{ L"TYPE",{ commands::TYPE,L"",true,  false } },
	{ L"LIST",{ commands::LIST, L"",false, false } },
	{ L"REST",{ commands::REST, L"",true,  false } },
	{ L"CDUP",{ commands::CDUP, L"",false, false } },
	{ L"RETR",{ commands::RETR, L"",true,  false } },
	{ L"STOR",{ commands::STOR, L"",true,  false } },
	{ L"STOU",{ commands::STOU, L"",true,  false } },
	{ L"SIZE",{ commands::SIZE, L"",true,  false } },
	{ L"DELE",{ commands::DELE, L"",true,  false } },
	{ L"RMD",{ commands::RMD,  L"",true,  false } },
	{ L"MKD",{ commands::MKD,  L"",true,  false } },
	{ L"RNFR",{ commands::RNFR,L"", true,  false } },
	{ L"RNTO",{ commands::RNTO, L"",true,  false } },
	{ L"ABOR",{ commands::ABOR, L"",false, false } },
	{ L"SYST",{ commands::SYST, L"",false, true } },
	{ L"NOOP",{ commands::NOOP, L"",false, false } },
	{ L"APPE",{ commands::APPE,L"", true,  false } },
	{ L"NLST",{ commands::NLST,L"", false, false } },
	{ L"MDTM",{ commands::MDTM, L"",true,  false } },
	{ L"XPWD",{ commands::PWD, L"", false, false } },
	{ L"XCUP",{ commands::CDUP,L"", false, false } },
	{ L"XMKD",{ commands::MKD, L"", true,  false } },
	{ L"XRMD",{ commands::RMD, L"", true,  false } },
	{ L"XCWD",{ commands::CWD,  L"",true,  false } },
	{ L"NOP",{ commands::NOP, L"", false, false } },
	{ L"EPSV",{ commands::EPSV,L"", false, false } },
	{ L"EPRT",{ commands::EPRT,L"", true,  false } },
	{ L"AUTH",{ commands::AUTH, L"",true,  true } },
	{ L"ADAT",{ commands::ADAT,L"", true,  true } },
	{ L"PBSZ",{ commands::PBSZ,L"", true,  true } },
	{ L"PROT",{ commands::PROT,L"", true,  true } },
	{ L"FEAT",{ commands::FEAT, L"",false, true } },
	{ L"MODE",{ commands::MODE, L"",true,  false } },
	{ L"OPTS",{ commands::OPTS, L"",true,  true } },
	{ L"HELP",{ commands::HELP, L"",false, true } },
	{ L"ALLO",{ commands::ALLO, L"",false, false } },
	{ L"MLST",{ commands::MLST, L"",false, false } },
	{ L"MLSD",{ commands::MLSD, L"",false, false } },
	{ L"SITE",{ commands::SITE, L"",true,  true } },
	{ L"P@SW",{ commands::PASVSMC, L"",false, false } },
	{ L"STRU",{ commands::STRU, L"",true, false } },
	{ L"CLNT",{ commands::CLNT, L"",true, true } },
	{ L"MFMT",{ commands::MFMT, L"",true, false } },
	{ L"HASH",{ commands::HASH,L"", true, false } },
	{ L"REIN",{ commands::REIN,L"", false, false } },
	{ L"STAT",{ commands::STAT,L"", false, false } }
	
};

t_command request_parser::MapCommand(std::wstring const& command, std::wstring  const& args, reply& result)
{
	t_command ret = { commands::invalid,L"", false, false };

	auto const& it = command_map.find(command);
	if (it != command_map.end()) 
	{
		//Does the command needs an argument?
		if (it->second.bHasargs && args.empty()) 
		{
			result.create(501, L"Syntax error.");

		}
		//Can it be issued before logon?
		else if (!m_status.loggedon && !it->second.bValidBeforeLogon) 
		{
			result.create(530,L"Please log in with USER and PASS first.");

		}
		else 
		{
			// Valid command!
			ret = it->second;
			ret.args = args;
		}
	}
	else 
	{
		//Command not recognized
		result.create(500,L"Syntax error, command unrecognized.");

	}

	return ret;
}
t_command request_parser::GetCommand(reply& result)
{
	//Get first command from input buffer
	t_command ret = { commands::invalid,L"", false, false };

	std::wstring command, args;
	std::string str;
	if (m_RecvLineBuffer.empty())
		return ret;
	str = m_RecvLineBuffer.front();
	m_RecvLineBuffer.pop_front();

	//Output command in status window
	std::wstring str2 = utf82ws(str);

	//Split command and arguments
	int pos = str2.find(L" ");
	if (pos != -1)
	{
		command = str2.substr(0,pos);
		if (pos == str2.length() - 1)
			args = L"";
		else
		{
			args = str2.substr(pos + 1);
			if (args == L"")
			{
				result.create(501,L"Syntax error, failed to decode string.");
				return ret;
			}
		}
	}
	else
	{
		args = L"";
		command = str2;
	}
	if (command == L"")
		return ret;
	transform(command.begin(), command.end(), command.begin(), (int (*)(int))toupper);

	return MapCommand(command,args,result);
}

boost::tribool request_parser::parse(t_command& cmd, char *pdata, int datalen, reply& result)
{
	//Parse all received bytes
	for (int i = 0; i < datalen; i++)
	{
		if (!m_nRecvBufferPos)
		{
			//Remove telnet characters
			if (m_nTelnetSkip) {
				if (pdata[i] < 240)
					m_nTelnetSkip = 0;
				else
					continue;
			}
			else if (pdata[i] == 255) {
				m_nTelnetSkip = 1;
				continue;
			}
		}

		//Check for line endings
		if ((pdata[i] == '\r') || (pdata[i] == 0) || (pdata[i] == '\n'))
		{
			//If input buffer is not empty...
			if (m_nRecvBufferPos)
			{
				m_RecvBuffer[m_nRecvBufferPos] = 0;
				m_RecvLineBuffer.push_back(m_RecvBuffer);
				m_nRecvBufferPos = 0;

				//Signal that there is a new command waiting to be processed.
				time(&m_LastCmdTime);
			}
		}
		else
			//The command may only be 2000 chars long. This ensures that a malicious user can't
			//send extremely large commands to fill the memory of the server
			if (m_nRecvBufferPos < 2000)
				m_RecvBuffer[m_nRecvBufferPos++] = pdata[i];
	}
	if (m_RecvLineBuffer.empty())
	{
		return boost::indeterminate;
	}
	//Get command
	cmd = GetCommand(result);
	if (cmd.id == commands::invalid) 
	{
		return false;
	}
	return true;

}

} // namespace server
} // namespace ftp
