//============================================================================
// Name        : jLog.cpp
// Author      : Jeremy C. Zacharia
// Version     :
// Copyright   : (C) Jeremy C. Zacharia
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <jLog.h>

int main()
{
	jLOG << "This is a logging message." << std::endl;
	jWAR << "This is a warning message." << std::endl;
	jERR << "This is an error  message." << std::endl;
	jDBG << "This is a debug message." << std::endl;
	return 0;
}
