#if !defined(LOGGER_H)
#define LOGGER_H

#include <string>
#include <iostream>
#include <ctime>

class Logger
{
	private:
		std::ostream& out;
		static Logger instance;

	public:
		static Logger& getInstance() { return instance; }
		Logger(std::ostream& os): out(os) {}

		enum log_level{
			OFF = -1,
			CRITICAL=0,
			ERROR=1,
			WARNING=2,
			INFO=3,
			DEBUG=4,
			TRACE=5
		};

		log_level current_log_level;


		void log(log_level level, std::string str)
		{
			if (level <= current_log_level)
				out << "[" << time(NULL) << "] " << str << std::endl;
		}

		void critical(std::string str) { log(CRITICAL, str); }
		void error   (std::string str) { log(ERROR   , str); }
		void warning (std::string str) { log(WARNING , str); }
		void info    (std::string str) { log(INFO    , str); }
		void debug   (std::string str) { log(DEBUG   , str); }
		void trace   (std::string str) { log(TRACE   , str); }
};

#define logger Logger::getInstance()

#endif
