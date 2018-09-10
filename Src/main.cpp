//============================================================================
// Name        : main.cpp
// Author      : Jeremy C. Zacharia
// Version     : 0.1.1
// Copyright   : (C) Jeremy C. Zacharia
// Description : jLog Threaded Example
//============================================================================

#include <jLog.h>
#include <future>

int main()
{
	jLog::init("", std::cout);

	std::function<void()> f1 = []() {
		jLog::log() << "This is a log message" << std::endl;
		jLog::log(Level::War) << "This\nis\na\nwarning\nmessage." << std::endl;
		jLog::log(Level::Err) << "This is an error message." << std::endl;
		jLog::log(Level::Dbg) << "This is a debug message." << std::endl;
	};

	std::function<void()> f2 = []() {
		jLog::log() << "This is another log message" << std::endl;
		jLog::log(Level::War) << "This \nis another wa\nrning message." << std::endl;
		jLog::log(Level::Err) << "This is another error message." << std::endl;
		jLog::log(Level::Dbg) << "This is another debug message." << std::endl;
	};

	std::async(f1);
	std::async(f2);

	return 0;
}
