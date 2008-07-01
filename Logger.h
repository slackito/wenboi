#if !defined(LOGGER_H)
#define LOGGER_H

#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include "Singleton.h"

class Logger: public Singleton<Logger>
{
	public:
		enum log_level{
			OFF = -1,
			CRITICAL=0,
			ERROR=1,
			WARNING=2,
			INFO=3,
			DEBUG=4,
			TRACE=5
		};

		Logger(): 
			out(std::cerr), 
			current_log_level(WARNING),
			log_start_time(time(NULL))
		{
			//log_start_time = time(NULL);
			out << "Starting log at " << static_cast<long>(log_start_time) << std::endl;
		}

		//void setOutput(std::ostream os) { out = os; }

		void log(log_level level, std::string str)
		{
			if (level <= current_log_level)
				out << "[" << difftime(time(NULL), log_start_time) << "] " << str << std::endl;
		}
		
		#define LOGFUNC1(func, tag) template <typename T1> void func(T1 p1) \
		{ std::stringstream str; str << p1;	log(tag, str.str()); }
		#define LOGFUNC2(func, tag) template <typename T1, typename T2> void func(T1 p1, T2 p2) \
		{ std::stringstream str; str << p1 << p2; log(tag, str.str()); }
		#define LOGFUNC3(func, tag) template <typename T1, typename T2, typename T3> \
		void func(T1 p1, T2 p2, T3 p3) \
		{ std::stringstream str; str << p1 << p2 << p3; log(tag, str.str()); }
		#define LOGFUNC4(func, tag) template <typename T1, typename T2, typename T3, typename T4> \
		void func(T1 p1, T2 p2, T3 p3, T4 p4) \
		{ std::stringstream str; str << p1 << p2 << p3 << p4; log(tag, str.str()); }
		#define LOGFUNC5(func, tag) template <typename T1, typename T2, typename T3, typename T4, typename T5> \
		void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5) \
		{ std::stringstream str; str << p1 << p2 << p3 << p4 << p5; log(tag, str.str()); }
		#define LOGFUNC6(func, tag) template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> \
		void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6) \
		{ std::stringstream str; str << p1 << p2 << p3 << p4 << p5 << p6; log(tag, str.str()); }
		#define LOGFUNC7(func, tag) template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> \
		void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7) \
		{ std::stringstream str; str << p1 << p2 << p3 << p4 << p5 << p6 << p7; log(tag, str.str()); }

		#define LOGFUNC(func, tag) \
			LOGFUNC1(func, tag) \
			LOGFUNC2(func, tag) \
			LOGFUNC3(func, tag) \
			LOGFUNC4(func, tag) \
			LOGFUNC5(func, tag) \
			LOGFUNC6(func, tag) \
			LOGFUNC7(func, tag)

		LOGFUNC(critical, CRITICAL)
		LOGFUNC(error,    ERROR)
		LOGFUNC(warning,  WARNING)
		LOGFUNC(info,     INFO)
		LOGFUNC(debug,    DEBUG)
		LOGFUNC(trace,    TRACE)

		void set_log_level (log_level level) { current_log_level = level; }
	private:
		std::ostream& out;
		log_level current_log_level;
		time_t log_start_time;
};

#define logger Logger::Instance()

#endif
