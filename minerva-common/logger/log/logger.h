/*
 The MIT License (MIT)
Copyright (c) 2013 <AthrunArthur>
Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

#ifndef FF_LOG_LOGGER_H_
#define FF_LOG_LOGGER_H_
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <exception>
#include <minerva/logger/log/logwriter.h> // By Jermaine
#include <minerva/logger/singlton.h> // By Jermaine

#if __cplusplus < 201103L
#include <boost/date_time/posix_time/posix_time.hpp>
#define BOOST_DATE_TIME_SOURCE
#else
#include <chrono>
#include <thread>
#endif
namespace ff
{
namespace internal
{
	template<bool EnableLogFlag> class Logger
	{
	public:
		virtual ~Logger()
		{
			FlushToWriter(false);
		}
	public:
		typedef Logger<EnableLogFlag> self;

		template<typename T> self& operator<<(T v)
		{
			buffer_ << v;
			return * this;
		}
		template<typename T> self& operator<<(T * p)
		{
			uintptr_t v = reinterpret_cast<uintptr_t>(p);
			buffer_<<"0x"<<std::hex<<v<<"  ";
			return *this;
		}
		self & operator<<(const char * p)
		{
			buffer_<<p;
			return *this;
		}
		self & operator<<(bool v)
		{
			buffer_<<(v ? "1": "0");
			return *this;
		}

	protected:
		void FlushToWriter(bool syncwriting)
		{
			try {
				std::stringstream ss;
#if __cplusplus < 201103L
				std::string str = boost::posix_time::to_simple_string(boost::posix_time::second_clock::local_time());
				ss << str << " | " << buffer_.str();
#else
				std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
				time_t now_c = std::chrono::system_clock::to_time_t(now);

				const char * s = asctime(std::localtime(&now_c));
				std::string str(s, std::strlen(s) -1);
				ss<<str<<"\t"<<std::this_thread::get_id()<<buffer_.str();
#endif
				if(syncwriting)
					ff::singleton<logwriter<> >::instance().flush(ss.str());
				else
					ff::singleton<logwriter<> >::instance().queue().push_back(ss.str());

			} catch(const std::exception & e)
			{
				std::cout << "Log Exception: " << e.what() << std::endl;
			}
		}
		std::stringstream  buffer_;

	};

	// Logger that won't output anything
	template<> class Logger<false>
	{
	public:
		typedef Logger<false> self;
		template<typename T> self& operator<<(T v) { return *this; }
		template<typename T> self& operator<< (T *v) { return *this; }
	};

}//end namespace internal
}//end namespace ff
#endif
