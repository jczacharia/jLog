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
#include <chrono>
#include <cstring>
#include <ctime>
#include <thread>
#include <functional>
#include <mutex>

/* Singleton Templated Class */
template<class T>
class Singleton
{
protected:
	/* Prevent instancing. */
	Singleton() noexcept = default;					// Constructor
	virtual ~Singleton() = default;					// Destructor
	Singleton(Singleton const&) = delete;            	// Copy construct
	Singleton(Singleton&&) = delete;                  	// Move construct
	Singleton& operator=(Singleton const&) = delete;  	// Copy assign
	Singleton& operator=(Singleton &&) = delete;      	// Move assign

public:
	static T& get()
	{
		static T instance;
		return instance;
	}
};

/* Available Logging Levels */
enum class Level
{
	Log, War, Err, Dbg
};

/* jLog Singleton */
class jLog: private Singleton<jLog>
{
	friend Singleton<jLog> ;

	/* Prevent instancing. */
	~jLog() = default;							// Destructor
	jLog(jLog const&) = delete;            	// Copy construct
	jLog(jLog&&) = delete;                  	// Move construct
	jLog& operator=(jLog const&) = delete;  	// Copy assign
	jLog& operator=(jLog &&) = delete;      	// Move assign

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
			// set flag so log isn't duplicated
			// 		for every << operation
			_new_log_entry = false;

			// Write to console
			*_console_output << jLog::getNowTime()
					<< jLog::logLevelToStringColor(_level) << str;

			// Write to file
			_file_output << jLog::getNowTime()
					<< jLog::logLevelToStringNoColor(_level) << str;
		} else {
			*_console_output << str; 	// Write to console
			_file_output << str; 		// Write to file
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
		jLog& jlog = jLog::get();
		jlog._level = l;
		return jlog;
	}

	static void init(const char* const p, std::ostream& os = std::cout)
	{
		std::lock_guard<std::mutex> guard(_mutex);
		jLog& jl = jLog::get();
		std::string file_path;

		file_path.append(p);

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
		std::cout << file_path << std::endl;

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
		switch(jll) {
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

#endif /* JLOG_H_ */
