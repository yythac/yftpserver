/*
* common.cpp
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#include "stdafx.h"
#include "common.h"


bool get_app_dir(std::wstring& app_dir)
{
	std::wstring fullpath = boost::filesystem::initial_path<boost::filesystem::path>().wstring();

	if (fullpath.empty() == true)
	{
		boost::filesystem::current_path(fullpath);
	}
	if (fullpath.empty() == false)
	{
		app_dir = fullpath;
		return true;
	}
	else
	{
		app_dir.clear();
		return false;
	}
}

bool simplify_path(wstring& _path)
{

	if (_path.empty() == true)
		return false;

	boost::system::error_code ec;
	boost::filesystem::path abpath = boost::filesystem::canonical(_path, ec);
	if (ec)
	{
		return false;
	}
	else
	{
		_path = abpath.generic_wstring();

		return true;
	}

}

bool get_file_status(const wstring& filename, boost::filesystem::file_status &st)
{
	boost::filesystem::path tmp(filename);
	boost::system::error_code ec;
	st = status(tmp, ec);
	if (ec)
	{
		// LOGI("getfilestatus %s error:%s", tmp.string().c_str(), ec.message().c_str());
	}
	return !ec;
}

/* Build a Path using the Client Command and the Client's User Start-Path. */
std::wstring build_path(const std::wstring start_path, const std::wstring current_path, std::wstring asked_path, bool bcanonical)
{
	boost::filesystem::path repath(L"");

	if (start_path.empty() == false && current_path.empty() == false)
	{
		if (asked_path.empty() == false)
		{
			if (asked_path[0] == L'/' || asked_path[0] == L'\\')
			{
				repath = boost::filesystem::path(start_path) / boost::filesystem::path(asked_path);
			}
			else
			{
				repath = boost::filesystem::path(start_path) / boost::filesystem::path(current_path) / boost::filesystem::path(asked_path);

			}

		}
		else
		{
			repath = boost::filesystem::path(start_path) / boost::filesystem::path(current_path);
		}
		if (bcanonical == true)
		{
			if (repath.wstring() == L"//")
				repath = L"/";
			boost::system::error_code ec;
			boost::filesystem::path abpath = canonical(repath, ec);
			if (!ec)
			{
				repath = abpath;
			}
		}
	}

	return repath.generic_wstring();
}
