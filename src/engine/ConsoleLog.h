#pragma once
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

enum ConsoleColor
{
	Black   = 0,
	Blue    = 1,
	Green   = 2,
	Cyan    = 3,
	Red     = 4,
	Magenta = 5,
	Yellow  = 6,
	White   = 7
};

class ConsoleLog
{
public:
	// Function to set text color in the console
	static void set(ConsoleColor color)
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
#else
		// Assume POSIX compliant terminal
		std::cout << "\033[1;" << color << "m";
#endif
	}

	// Function to reset text color in the console
	static void reset()
	{
#ifdef _WIN32
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(ConsoleColor::White));
#else
		// Assume POSIX compliant terminal
		std::cout << "\033[0m";
#endif
	}

	// Function to log a message with a specified color
	static void log(const std::string& message, ConsoleColor color)
	{
		set(color);
		std::cout << message << std::endl;
		reset();
	}
};