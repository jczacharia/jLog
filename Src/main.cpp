//============================================================================
// Name        : jLog.cpp
// Author      : Jeremy C. Zacharia
// Version     :
// Copyright   : (C) Jeremy C. Zacharia
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <jLog.h>

#define jLOG jLog::get()->log(_j_LOG__)
#define jWAR jLog::get()->log(_j_WAR__)
#define jERR jLog::get()->log(_j_ERR__)
#define jDBG jLog::get()->log(_j_DBG__)

int main()
{
	jLOG << "This is a logging message." << std::endl;
	jWAR << "This is a warning message." << std::endl;
	jERR << "This is an error  message." << std::endl;
	jDBG << "This is a debug message." << std::endl;

	std::cin.get();
	return 0;
}
