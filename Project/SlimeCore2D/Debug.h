#pragma once
#include <string>

class Debug
{
public:
	static void PrintError(std::string message);
	static void PrintWarning(std::string message);
	static void PrintLog(std::string message);
	static void PrintCreate(std::string message);
	static void PrintDelete(std::string message);
};
