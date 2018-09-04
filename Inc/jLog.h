//============================================================================
// Name        : jLog.h
// Author      : Jeremy C. Zacharia
// Version     : 0.1.1
// Copyright   : (C) Jeremy C. Zacharia
// Description : jLog Threaded Example
//============================================================================


#ifndef JLOG_H_
#define JLOG_H_

#include <iostream>
#include <fstream>
#include <memory>
#include <chrono>
#include <cstring>
#include <ctime>
#include <thread>
#include <mutex>

enum class Level
{
	Log, War, Err, Dbg
};

template<class T>
class Singleton
{
protected:
	/* Here will be the instance stored. */
	static std::unique_ptr<T> instance;

	/* Prevent instancing. */
	Singleton() noexcept = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	virtual ~Singleton() = default;

public:
	/* Static access method. */
	static const std::unique_ptr<T>& get()
			noexcept(std::is_nothrow_constructible<T>::value);

};

class jLog: private Singleton<jLog>
{
public:
	jLog() :
					_level(Level::Log),
					_time_stamp_flag_for_new_log_entry(true),
					_console_output(&std::cout)
	{
		// Get Now Time
		auto now_time = std::chrono::system_clock::now();
		std::time_t t_time = std::chrono::system_clock::to_time_t(now_time);
		tm local_time = *localtime(&t_time);

		// Create new single log file for the program's execution
		std::string fmt_time = "logs/jLog_";
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

		_file_output = std::ofstream(fmt_time, std::ios::out | std::ios::app);
		if(!_file_output.is_open()) {
			// std::cerr writes (typically error messages) to the standard error stream stderr (unbuffered)
			// https://en.wikipedia.org/wiki/Stderr
			std::cerr << "*** jLog error: could not open output file: "
					<< fmt_time << std::endl;
		} else {
			*_console_output << "Created log file: " << fmt_time << std::endl;
			_file_output
					<< "This Log file was created using the jLog Framework (C) Jeremy C Zacharia\n\n";
		}
	}

	~jLog()
	{
		_file_output.close();
	}

	const char* const getNowTime()
	{
		auto now_time = std::chrono::system_clock::now();
		std::time_t sleep_time = std::chrono::system_clock::to_time_t(now_time);
		char* rtn = std::ctime(&sleep_time);
		rtn[strlen(rtn) - 1] = '\0'; // delete '\n' at the end
		return rtn;
	}

	template<typename T>
	jLog& operator<<(const T& t)
	{
		std::lock_guard<std::mutex> lock(jLog::_mutex);
		if(_time_stamp_flag_for_new_log_entry) {
			// set flag false so time and log isn't displayed
			// 		for every << operation
			_time_stamp_flag_for_new_log_entry = false;

			// Write to console
			*_console_output << jLog::getNowTime()
					<< jLog::logLevelToStringColor(_level) << t;

			// Write to file
			_file_output << jLog::getNowTime()
					<< jLog::logLevelToStringNoColor(_level) << t;
		} else {
			// Write to console
			*_console_output << t;

			// write to file
			_file_output << t;
		}
		return *this;
	}

	// used to flag that std::endl has been called
	//     to designate log entry is finished
	jLog& operator<<(std::ostream& (*os)(std::ostream&))
	{
		std::lock_guard<std::mutex> lock(jLog::_mutex);

		// Write new line to console
		*_console_output << os;

		// Write new line at the file for each log entry
		_file_output << os;
		_time_stamp_flag_for_new_log_entry = true;
		return *this;
	}

	/*
	 *  Static members
	 */
	static jLog& log(Level l = Level::Log)
	{
		std::lock_guard<std::mutex> lock(jLog::_mutex);
		jLog& jlog = *jLog::get();
		jlog._level = l;
		return jlog;
	}

	static void setConsoleOutput(std::ostream& os)
	{
		jLog& jlog = *jLog::get();
		jlog._console_output = &os;
	}

	static void setFileOutput(const char* const path)
	{
		jLog& jlog = *jLog::get();
		jlog._file_output = std::ofstream(path, std::ios::out | std::ios::app);
	}

private:

	Level _level;
	bool _time_stamp_flag_for_new_log_entry;
	std::ofstream _file_output;
	std::ostream* _console_output;
	static std::mutex _mutex;

	const char* const logLevelToStringColor(Level jll)
	{
		switch(_level) {
		case Level::Log:
			return " [   LOG   ] ";
		case Level::War:
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
		case Level::War:
			return " [ WARNING ] ";
		case Level::Err:
			return " [  ERROR  ] ";
		case Level::Dbg:
			return " [  DEBUG  ] ";
		}
		return "";
	}
};

// Init mutex
std::mutex jLog::_mutex;

// Init static Singleton instance member
template<typename T>
std::unique_ptr<T> Singleton<T>::instance(nullptr);

/* Static access method. */
template<typename T>
const std::unique_ptr<T>& Singleton<T>::get()
noexcept(std::is_nothrow_constructible<T>::value)
{
	if(!instance) {
		instance = std::make_unique<T>();
	}
	return instance;
}

#endif /* JLOG_H_ */
