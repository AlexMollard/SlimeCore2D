#pragma once
#include <string>

class Debug
{
public:
	static void PrintError(std::string message);
	static void PrintWarning(std::string message);
	static void PrintLog(std::string message);
};

