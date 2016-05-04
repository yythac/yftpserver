/*
* error_code.h
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/

#pragma once
#define YFTP_ERROR_CODE_BEGIN			0xE0000000

#define YFTP_ERROR_USER_NOT_FOUND		(YFTP_ERROR_CODE_BEGIN+1)
#define YFTP_ERROR_USER_EXIST			(YFTP_ERROR_CODE_BEGIN+2)
#define YFTP_ERROR_PATH_INVALID			(YFTP_ERROR_CODE_BEGIN+3)
#define YFTP_ERROR_PARAM_INVALID		(YFTP_ERROR_CODE_BEGIN+4)
#define YFTP_ERROR_USERNAME_INVALID		(YFTP_ERROR_CODE_BEGIN+4)

#include <string>

using namespace std;

namespace ftp {
	namespace server {

		class error_manager
		{
		public:
			error_manager()
			{

			}
			error_manager(int error)
			{
				set_error_code(error);
			}

			void set_error_code(int error)
			{
				error_code_ = error;
			}
			int get_error_code()
			{
				return error_code_;
			}
			wstring to_string()
			{
				return L"unknow error!";
			}
		private:

			int error_code_;

		};
	}
}