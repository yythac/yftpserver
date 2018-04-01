/*
* reply.h 
*
* Mail :: yyt_hac@163.com
*
*Copyright (c) 2015 yyt_hac
*/
//

#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/format.hpp>

#include "conversion.h"


namespace ftp {
	namespace server {

		enum class codetype
		{
			utf8,
			gbk
		};

		/// A reply to be sent to a client.
		class reply
		{
		public:

			reply()
			{
				code_type_ = codetype::utf8;
				code_ = 0;
				content_.clear();
			}
			reply(unsigned short code,const std::wstring& content, codetype code_type)
				: reply()
			{
				create(code, content, code_type);
			}
			void create(unsigned short code, const std::wstring& content)
			{
				create(code, content, code_type_);
			}
			void create(unsigned short code, const std::wstring& content, codetype code_type)
			{
				code_type_ = code_type;
				code_ = code;

				if (code_type_ == codetype::gbk)
				{
					content_ = ws2s(content);
				}
				else
				{
					content_ = ws2utf8(content);
				}

				if (code_ > 0)
				{
					content_ = (boost::format("%d %s\r\n") % code_ % content_).str();
				}
			}

			bool empty()
			{
				return content_.empty();
			}
			void set_code_type(codetype code_type)
			{
				code_type_ = code_type;
			}
			std::string to_string()
			{
				return content_;
			}
			/// Convert the reply into a vector of buffers. The buffers do not own the
			/// underlying memory blocks, therefore the reply object must remain valid and
			/// not be changed until the write operation has completed.
			boost::asio::const_buffers_1 to_buffer()
			{
				return boost::asio::buffer(content_.data(), content_.size());
			}

		private:
			//reply code
			unsigned short code_;
			/// The content to be sent in the reply.
			std::string content_;

			codetype code_type_;

		};

	} // namespace server
} // namespace ftp


