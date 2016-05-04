/*
* common.h 
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#pragma once

#include <string>

#include <boost/filesystem.hpp>

using namespace std;

bool get_app_dir(wstring& app_dir);
bool simplify_path(wstring& _path);
bool get_file_status(const wstring& filename, boost::filesystem::file_status &st);
std::wstring build_path(const std::wstring start_path, const std::wstring current_path, std::wstring asked_path, bool bcanonical = true);