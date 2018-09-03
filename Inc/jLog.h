/*
 * jLog.h
 *
 *  Created on: Aug 26, 2018
 *      Author: jeremy
 */

#ifndef JLOG_H_
#define JLOG_H_

#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <ctime>

enum class Level {
	Log, Warn, Err, Dbg
};

template<class T>
class Singleton {
protected:
	/* Here will be the instance stored. */
	static std::unique_ptr<T> instance;

	/* Protected constructor to prevent instancing. */
	Singleton()
	{
	}

public:
	/* Static access method. */
	static std::unique_ptr<T>& get()
			noexcept(std::is_nothrow_constructible<T>::value);
};

class jLog: private Singleton<jLog> {
public:

	jLog() :
			_level(Level::Log), _logTFlag(true)
	{
		// Get Now Time
		auto now_time = std::chrono::system_clock::now();
		std::time_t t_time = std::chrono::system_clock::to_time_t(now_time);
		tm local_time = *localtime(&t_time);

		// Create new single log file for the program's execution
		std::string fmt_time = "logs/";
		fmt_time.append(std::to_string(local_time.tm_year + 1900));
		fmt_time.append("-");
		fmt_time.append(std::to_string(local_time.tm_mon));
		fmt_time.append("-");
		fmt_time.append(std::to_string(local_time.tm_mday));
		fmt_time.append("_");
		fmt_time.append(std::to_string(local_time.tm_hour));
		fmt_time.append("-");
		fmt_time.append(std::to_string(local_time.tm_min));
		fmt_time.append("-");
		fmt_time.append(std::to_string(local_time.tm_sec));
		fmt_time.append(".txt");

		_ofs = std::ofstream(fmt_time, std::ios::out | std::ios::app);
		if(!_ofs.is_open()) {
			// std::cerr writes (typically error messages) to the standard error stream stderr (unbuffered)
			// https://en.wikipedia.org/wiki/Stderr
			std::cerr << "*** error: could not open output file: " << fmt_time
					<< std::endl;
		} else {
			std::cout << "Created log file: " << fmt_time << std::endl;
		}
	}

	~jLog()
	{
		_ofs.close();
	}

public:

	std::string getNowTime()
	{
		auto now_time = std::chrono::system_clock::now();
		std::time_t sleep_time = std::chrono::system_clock::to_time_t(now_time);
		std::string tmp_time = std::string(std::ctime(&sleep_time));
		tmp_time.pop_back();
		return tmp_time;
	}

	template<typename T>
	jLog& operator<<(const T& t)
	{
		if(_logTFlag) {

			// set flag false so time and log isn't displayed
			// 		for every << operation
			_logTFlag = false;

			// Write to console
			std::cout << jLog::getNowTime() << jLog::logLevelToString(_level)
					<< t;
			// write to file
			_ofs << jLog::getNowTime() << jLog::logLevelToStringNoColor(_level)
					<< t;
		} else {

			// Write to console
			std::cout << t;

			// write to file
			_ofs << t;
		}
		return *this;
	}

	// used for multiple << operations on each write
	template<Level l>
	static jLog& log()
	{
		jLog& rtn = *jLog::get();
		rtn._level = l;
		return rtn;
	}

	// used to flag that std::endl has been called
	//     to designate log entry is finished
	jLog& operator<<(std::ostream& (*os)(std::ostream&))
	{
		// Write new line to console
		std::cout << os;
		// Write new line at the file for each log entry
		_ofs << os;
		_logTFlag = true;
		return *this;
	}

private:

	Level _level;
	bool _logTFlag;
	std::ofstream _ofs;
	//std::ostream _output;

	const char* const logLevelToString(Level jll)
	{
		switch(_level) {
		case Level::Log:
			return " [   LOG   ] ";
		case Level::Warn:
			return " [ \e[33mWARNING\e[00m ] ";
		case Level::Err:
			return " [  \e[31mERROR\e[00m  ] ";
		case Level::Dbg:
			return " [  \e[34mDEBUG\e[00m  ] ";
		}
		return "";
	}

	const char* const logLevelToStringNoColor(Level jll)
	{
		switch(jll) {
		case Level::Log:
			return " [   LOG   ] ";
		case Level::Warn:
			return " [ WARNING ] ";
		case Level::Err:
			return " [  ERROR  ] ";
		case Level::Dbg:
			return " [  DEBUG  ] ";
		}
		return "";
	}
};

// initialize static Singleton member
template<typename T>
std::unique_ptr<T> Singleton<T>::instance(nullptr);

/* Static access method. */
template<typename T>
std::unique_ptr<T>& Singleton<T>::get()
noexcept(std::is_nothrow_constructible<T>::value)
{
	if(!instance) {
		instance = std::make_unique<T>();
	}
	return instance;
}

#endif /* JLOG_H_ */
