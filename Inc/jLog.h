//============================================================================
// Name        : jLog.h
// Author      : Jeremy C. Zacharia
// Version     : 0.1.2
// Copyright   : (C) Jeremy C. Zacharia
// Description : jLog Logging Framework
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
#include <functional>
#include <mutex>
#include <cstdlib>
#include <atomic>
#include <exception>

enum class Level
{
	Log, War, Err, Dbg
};

template<class T>
class Singleton
{
protected:
	/* Here will be the instance stored. */
	static std::atomic<T*> instance;
	static std::mutex mutex;

	/* Prevent instancing. */
	Singleton() noexcept = default;
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	virtual ~Singleton() = default;

public:
	/* Static access method. */
	static T* get() noexcept(std::is_nothrow_constructible<T>::value);

};

class jLog: private Singleton<jLog>
{
	friend Singleton<jLog> ;
	jLog(const jLog&) = delete;
	jLog& operator=(const jLog&) = delete;

	jLog() :
					_level(Level::Log),
					_new_log_entry(true),
					_console_output(&std::cout)
	{
	}

public:
	template<typename T>
	jLog& operator<<(const T& t)
	{
		std::string str(t);
		findAndReplace(str, "\n", "\n\t\t\t\t     ");

		if(_new_log_entry) {
			// set flag false so time and log isn't displayed
			// 		for every << operation
			_new_log_entry = false;

			// Write to console
			*_console_output << jLog::getNowTime()
					<< jLog::logLevelToStringColor(_level) << str;

			// Write to file
			_file_output << jLog::getNowTime()
					<< jLog::logLevelToStringNoColor(_level) << str;
		} else {
			// Write to console
			*_console_output << str;

			// write to file
			_file_output << str;
		}
		return *this;
	}

	// used to flag that std::endl has been called
	//     to designate log entry is finished
	jLog& operator<<(std::ostream& (*os)(std::ostream&))
	{

		// Write new line to console
		*_console_output << os;

		// Write new line at the file for each log entry
		_file_output << os;
		_new_log_entry = true;
		_mutex.unlock();
		return *this;
	}

	/*
	 *  Static members
	 */
	static jLog& log(Level l = Level::Log)
	{
		_mutex.lock();
		jLog& jlog = *jLog::get();
		jlog._level = l;
		return jlog;
	}

	static void init(const char* const p = "", std::ostream& os = std::cout)
	{
		std::lock_guard<std::mutex> guard(_mutex);
		jLog& jl = *jLog::get();
		std::string file_path;

		file_path.append("logs/");

		if(strcmp(p, "") == 0) {
			// Get Now Time
			auto now_time = std::chrono::system_clock::now();
			std::time_t t_time = std::chrono::system_clock::to_time_t(now_time);
			tm local_time = *localtime(&t_time);

			file_path.append(std::to_string(local_time.tm_year + 1900));
			file_path.append("-");
			file_path.append(std::to_string(local_time.tm_mon));
			file_path.append("-");
			file_path.append(std::to_string(local_time.tm_mday));
			file_path.append("_");
			file_path.append(std::to_string(local_time.tm_hour));
			file_path.append("-");
			file_path.append(std::to_string(local_time.tm_min));
			file_path.append("-");
			file_path.append(std::to_string(local_time.tm_sec));
			file_path.append(".txt");
		} else {
			file_path = p;
		}

		jl._console_output = &os;
		jl._file_output = std::ofstream(p, std::ios::out | std::ios::app);

		jl._file_output = std::ofstream(file_path,
				std::ios::out | std::ios::app);
		if(!jl._file_output.is_open()) {
			std::cerr << "*** jLog error: could not open output file: "
					<< file_path << std::endl;
		} else {
			*jl._console_output << "Created log file: " << file_path << "\n\n";
			jl._file_output
			<< "This Log file was created "
					<< "using the jLog Framework (C) Jeremy C Zacharia\n\n";
		}
	}

private:

	Level _level;
	bool _new_log_entry;
	std::ofstream _file_output;
	std::ostream* _console_output;
	static std::mutex _mutex;

	void findAndReplace(std::string& source, std::string const& find,
			std::string const& replace)
	{
		for(std::string::size_type i = 0;
				(i = source.find(find, i)) != std::string::npos;)
				{
			source.replace(i, find.length(), replace);
			i += replace.length();
		}
	}

	const char* const getNowTime()
	{
		auto now_time = std::chrono::system_clock::now();
		std::time_t sleep_time = std::chrono::system_clock::to_time_t(now_time);
		char* rtn = std::ctime(&sleep_time);
		rtn[strlen(rtn) - 1] = '\0'; // delete '\n' at the end
		return rtn;
	}

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
		return " [   LOG   ] ";
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
		return " [   LOG   ] ";
	}
};

std::mutex jLog::_mutex;

template<typename T>
std::mutex Singleton<T>::mutex;

template<typename T>
std::atomic<T*> Singleton<T>::instance;

/* Static access method. */
template<typename T>
T* Singleton<T>::get()
noexcept(std::is_nothrow_constructible<T>::value)
{
	T* sin = instance.load();
	if(!sin) {
		std::lock_guard<std::mutex> myLock(mutex);
		sin = instance.load();
		if(!sin) {
			sin = new T;
			instance.store(sin);
		}
	}
	return sin;
}

#endif /* JLOG_H_ */
