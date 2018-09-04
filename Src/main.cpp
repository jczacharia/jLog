//============================================================================
// Name        : main.cpp
// Author      : Jeremy C. Zacharia
// Version     : 0.1.1
// Copyright   : (C) Jeremy C. Zacharia
// Description : jLog Threaded Example
//============================================================================

#include <jLog.h>

void f1()
{
	jLog::log() << "This is a log message" << std::endl;
	jLog::log(Level::War) << "This is a warning message." << std::endl;
	jLog::log(Level::Err) << "This is an error message." << std::endl;
	jLog::log(Level::Dbg) << "This is a debug message." << std::endl;
}

void f2()
{
	jLog::log() << "This is another log message" << std::endl;
	jLog::log(Level::War) << "This is another warning message." << std::endl;
	jLog::log(Level::Err) << "This is another error message." << std::endl;
	jLog::log(Level::Dbg) << "This is another debug message." << std::endl;

}

int main()
{
	jLog::setConsoleOutput(std::cerr);

	std::thread t1(f1);
	std::thread t2(f2);

	t1.join();
	t2.join();

	return 0;
}
