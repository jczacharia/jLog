/*
 * jLog.h
 *
 *  Created on: Aug 26, 2018
 *      Author: jeremy
 */

#ifndef JLOG_H_
#define JLOG_H_

#include <iostream>
#include <boost/filesystem.hpp>
#include <chrono>
#include <ctime>

enum jLogLevel
{
	_j_LOG__, _j_WAR__, _j_ERR__, _j_DBG__
};

#define jLOG jLog::get()->log(_j_LOG__)
#define jWAR jLog::get()->log(_j_WAR__)
#define jERR jLog::get()->log(_j_ERR__)
#define jDBG jLog::get()->log(_j_DBG__)

template<class T>
class Singleton
{
protected:
	/* Here will be the instance stored. */
	static std::shared_ptr<T> instance;

	/* Protected constructor to prevent instancing. */
	Singleton()
	{
	}

public:
	/* Static access method. */
	static std::shared_ptr<T> get()
			noexcept(std::is_nothrow_constructible<T>::value);
};

class jLog: public Singleton<jLog>
{
public:

	jLog()
	{
		// Get Now Time
		auto now_time = std::chrono::system_clock::now();
		std::time_t t_time = std::chrono::system_clock::to_time_t(now_time);
		tm local_time = *localtime(&t_time);

		// Create logs directory to store log files
		const char d[] = "../logs";
		boost::filesystem::path dir(d);
		try {
			boost::filesystem::create_directory(dir);
		} catch(boost::filesystem::filesystem_error &e) {
			std::cerr << e.what() << '\n';
		}

		// Create new single log file for the program's execution
		std::string fmt_time = "../logs/";
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

		std::cout << "Created log file: " << fmt_time << std::endl;

		// Init ofstream to append to log file
		_p = boost::filesystem::path(fmt_time);
		_ofs = std::make_unique<boost::filesystem::ofstream>(_p);
	}

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
		if(logTFlag) {

			// set flag false so time and log isn't displayed
			// 		for every << operation
			logTFlag = false;

			// Write to console
			std::cout << jLog::getNowTime() << jLog::logLevelToString(_jll)
					<< t;
			// write to file
			*_ofs << jLog::getNowTime() << jLog::logLevelToStringNoColor(_jll)
					<< t;
		} else {

			// Write to console
			std::cout << t;

			// write to file
			*_ofs << t;
		}

		// Write new line at the file for each log entry
		*_ofs << "\n";
		return *this;
	}

	// used for multiple << operations on each write
	jLog& log(jLogLevel jll)
	{
		_jll = jll;
		return *this;
	}

	// used to flag that std::endl has been called
	//     to designate log entry is finished
	jLog& operator<<(std::ostream& (*os)(std::ostream&))
	{
		std::cout << os;
		logTFlag = true;
		return *this;
	}

private:

	jLogLevel _jll = _j_LOG__;
	bool logTFlag = true;

	boost::filesystem::path _p;
	std::unique_ptr<boost::filesystem::ofstream> _ofs;

	std::string logLevelToString(jLogLevel jll)
	{
		switch(jll) {
		case _j_LOG__:
			return " [   LOG   ] ";
		case _j_WAR__:
			return " [ \e[33mWARNING\e[00m ] ";
		case _j_ERR__:
			return " [  \e[31mERROR\e[00m  ] ";
		case _j_DBG__:
			return " [  \e[34mDEBUG\e[00m  ] ";
		}
		return "";
	}
	std::string logLevelToStringNoColor(jLogLevel jll)
	{
		switch(jll) {
		case _j_LOG__:
			return " [   LOG   ] ";
		case _j_WAR__:
			return " [ WARNING ] ";
		case _j_ERR__:
			return " [  ERROR  ] ";
		case _j_DBG__:
			return " [  DEBUG  ] ";
		}
		return "";
	}
};

// initialize static Singleton member
template<typename T>
std::shared_ptr<T> Singleton<T>::instance(nullptr);

/* Static access method. */
template<typename T>
std::shared_ptr<T> Singleton<T>::get()
noexcept(std::is_nothrow_constructible<T>::value)
{
	if(!instance.get()) {
		instance = std::make_shared<T>();
	}
	return instance;
}

#endif /* JLOG_H_ */
