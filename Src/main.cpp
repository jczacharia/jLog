//============================================================================
// Name        : main.cpp
// Author      : Jeremy C. Zacharia
// Version     :
// Copyright   : (C) Jeremy C. Zacharia
// Description : jLog Example
//============================================================================

#include <jLog.h>

int main()
{
	jLog::log<Level::Log>() << "SDf" << std::endl;

	//jWAR << "This is a warning message." << std::endl;
	//jERR << "This is an error message." << std::endl;
	//jDBG << "This is a debug message." << std::endl;

	return 0;
}
