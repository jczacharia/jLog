/*
 * gtestx_test.cpp
 *
 *  Created on: Sep 10, 2018
 *      Author: jeremy
 */

#include "gtestx.h"
#include <gtest/gtest.h>
#include <future>
#include "jLog.h"

TEST(JLOG_TEST, init)
{
	jLog::init("", std::cout);
}

TEST(JLOG_TEST, noInitLog)
{

	jLog::log(Level::Log) << "This is a log message" << std::endl;
	jLog::log(Level::War) << "This is a warning message." << std::endl;
	jLog::log(Level::Err) << "This is an error message." << std::endl;
	jLog::log(Level::Dbg) << "This is a debug message." << std::endl;
}

TEST(JLOG_TEST, initLog)
{
	jLog::init("", std::cout);

	jLog::log(Level::Log) << "This is a log message" << std::endl;
	jLog::log(Level::War) << "This is a warning message." << std::endl;
	jLog::log(Level::Err) << "This is an error message." << std::endl;
	jLog::log(Level::Dbg) << "This is a debug message." << std::endl;

}

TEST(JLOG_TEST, noInitLogAsync)
{
	std::function<void()> f1 =
			[]() {
				jLog::log(Level::Log) << "This is a log message" << std::endl;
				jLog::log(Level::War) << "This is a warning message." << std::endl;
				jLog::log(Level::Err) << "This is an error message." << std::endl;
				jLog::log(Level::Dbg) << "This is a debug message." << std::endl;
			};

	std::function<void()> f2 =
			[]() {
				jLog::log(Level::Log) << "This is another log message" << std::endl;
				jLog::log(Level::War) << "This is another warning message." << std::endl;
				jLog::log(Level::Err) << "This is another error message." << std::endl;
				jLog::log(Level::Dbg) << "This is another debug message." << std::endl;
			};

	std::async(f1);
	std::async(f2);
}

TEST(JLOG_TEST, initLogAsync)
{
	jLog::init("", std::cout);

	std::function<void()> f1 =
			[]() {
				jLog::log(Level::Log) << "This is a log message" << std::endl;
				jLog::log(Level::War) << "This is a warning message." << std::endl;
				jLog::log(Level::Err) << "This is an error message." << std::endl;
				jLog::log(Level::Dbg) << "This is a debug message." << std::endl;
			};

	std::function<void()> f2 =
			[]() {
				jLog::log(Level::Log) << "This is another log message" << std::endl;
				jLog::log(Level::War) << "This is another warning message." << std::endl;
				jLog::log(Level::Err) << "This is another error message." << std::endl;
				jLog::log(Level::Dbg) << "This is another debug message." << std::endl;
			};

	std::async(f1);
	std::async(f2);
}
