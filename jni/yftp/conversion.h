#pragma once

/*
* conversion.h 
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/


#include <string>

std::string ws2s(const std::wstring& ws);
std::wstring s2ws(const std::string& s);
std::string ws2utf8_1(const std::wstring& ws);
std::wstring utf82ws_1(const std::string& s);
const std::string ws2utf8(const std::wstring& src);
const std::wstring utf82ws(const std::string& src);


