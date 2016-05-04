/*
* conversion.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#include "stdafx.h"
#include "conversion.h"

#include <cstring>
#include <locale>
#include <codecvt>

std::string ws2s(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}
std::wstring s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

std::string ws2utf8_1(const std::wstring& ws)
{
	std::string curLocale = setlocale(LC_ALL, nullptr); // curLocale = "C";
	setlocale(LC_ALL, "utf8");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	wcstombs(_Dest, _Source, _Dsize);
	std::string result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}
std::wstring utf82ws_1(const std::string& s)
{
	setlocale(LC_ALL, "utf8");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

const std::string ws2utf8(const std::wstring& src)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
	return conv.to_bytes(src);
}

const std::wstring utf82ws(const std::string& src)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t > conv;
	return conv.from_bytes(src);
}
/*
CStdStringW ConvFromNetwork(const char* buffer)
{
	int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buffer, -1, 0, 0);
	if (len)
	{
		CStdStringW str;
		wchar_t* out = str.GetBuffer(len + 2);
		len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, buffer, -1, out, len + 1);
		str.ReleaseBuffer();
		if (!len)
			str = L"";
		return str;
	}
	len = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, buffer, -1, 0, 0);
	if (len)
	{
		CStdStringW str;
		wchar_t* out = str.GetBuffer(len + 2);
		len = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, buffer, -1, out, len + 1);
		str.ReleaseBuffer();
		if (!len)
			str = L"";
		return str;
	}

	return L"";
}

std::string ConvToNetwork(const CStdStringW& str)
{
	std::string ret;
	int len = WideCharToMultiByte(CP_UTF8, 0, str, -1, 0, 0, 0, 0);
	if (len > 0) {
		char* output = new char[len + 2];
		if (WideCharToMultiByte(CP_UTF8, 0, str, -1, output, len + 1, 0, 0) > 0)
			ret = output;
		delete [] output;
	}

	return ret;
}

CStdStringA ConvToLocal(const CStdStringW& str)
{
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, 0, 0, 0, 0);
	if (!len)
		return "";

	CStdStringA outStr;
	char* output = outStr.GetBuffer(len + 2);
	if (!WideCharToMultiByte(CP_ACP, 0, str, -1, output, len + 1, 0, 0))
	{
		output[0] = 0;
		outStr.ReleaseBuffer();
		return "";
	}
	outStr.ReleaseBuffer();

	return outStr;
}

CStdStringA ConvToLocal(const CStdStringA& str)
{
	return str;
}

CStdStringW ConvFromLocal(const CStdStringA& local)
{
	int len = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, local, -1, 0, 0);
	if (len)
	{
		CStdStringW str;
		wchar_t* out = str.GetBuffer(len + 2);
		len = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, local, -1, out, len + 1);
		str.ReleaseBuffer();
		if (!len)
			str = L"";
		return str;
	}

	return L"";
}
*/
