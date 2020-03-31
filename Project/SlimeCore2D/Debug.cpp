#include "Debug.h"
#include <iostream>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"

void Debug::PrintError(std::string message)
{
	std::cout << RED << "ERROR: " << RESET << message << std::endl;
}

void Debug::PrintWarning(std::string message)
{
	std::cout << YELLOW << "WARNING: " << RESET << message  << std::endl;
}

void Debug::PrintLog(std::string message)
{
	std::cout << CYAN << "LOG: " << RESET << message << std::endl;
}
