#include "Debug.h"
#include <iostream>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define PURPLE  "\033[35m"
#define CYAN    "\033[36m"

void Debug::PrintError(std::string message)
{
	std::cout << RED << "Error: " << RESET << message << std::endl;
}

void Debug::PrintWarning(std::string message)
{
	std::cout << YELLOW << "Warning: " << RESET << message << std::endl;
}

void Debug::PrintLog(std::string message)
{
	std::cout << CYAN << "Log: " << RESET << message << std::endl;
}

void Debug::PrintCreate(std::string message)
{
	static int createTotal = 0;
	createTotal++;

	if (createTotal < 9)
	{
		std::cout << GREEN << "Created: " << RESET << message << std::endl;
	}
}

void Debug::PrintDelete(std::string message)
{
	static int deleteTotal = 0;
	deleteTotal++;

	if (deleteTotal < 9)
	{
		std::cout << PURPLE << "Deleted: " << RESET << message << std::endl;
	}
}