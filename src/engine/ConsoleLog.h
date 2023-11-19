#pragma once
#include <iomanip>
#include <iostream>
#include <vector>
#include <assert.h>

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

	// Overloaded function to log a list with a specified color
	template<typename T>
	static void log(const std::string& listName, const std::vector<T>& list, ConsoleColor headerColor, ConsoleColor listColour)
	{
		set(headerColor);
		std::cout << listName << ":" << std::endl;
		
		set(listColour);
		for (const auto& item : list)
		{
			std::cout << "  " << item << std::endl;
		}
		reset();
	}

	// Overloaded function to log a formatted message with a specified color
	template<typename... Args>
	static void logFormatted(ConsoleColor color, const std::string& format, Args... args)
	{
		set(color);
		std::cout << std::format(format, args...) << std::endl;
		reset();
	}

	// Function for debugging with file and line information (INFO)
	static void internalInfo(const std::string& message, const char* file, int line)
	{
		ConsoleLog::set(ConsoleColor::Cyan);
		std::cout << "INFO";
		ConsoleLog::reset();

		std::cout << std::format(" : {}({}) {}", file, line, message) << std::endl;
	}

	// Function for debugging with file and line information (WARN)
	static void internalWarn(const std::string& message, const char* file, int line)
	{
		ConsoleLog::set(ConsoleColor::Yellow);
		std::cout << "WARN";
		ConsoleLog::reset();

		std::cout << std::format(" : {}({}) {}", file, line, message) << std::endl;
	}

	// Function for debugging with file and line information (ERROR)
	static void internalError(const std::string& message, const char* file, int line)
	{
		ConsoleLog::set(ConsoleColor::Red);
		std::cout << "ERROR";
		ConsoleLog::reset();

		std::cout << std::format(" : {}({}) {}", file, line, message) << std::endl;
	}
};

// Debug macro for convenience
#define SLIME_INFO(message)  ConsoleLog::internalInfo(message, __FILE__, __LINE__)
#define SLIME_WARN(message)  ConsoleLog::internalWarn(message, __FILE__, __LINE__)

#define SLIME_ERROR(message)                                                                                                                                                  \
	do                                                                                                                                                                        \
	{                                                                                                                                                                         \
		ConsoleLog::internalError(message, __FILE__, __LINE__);                                                                                                               \
		assert(false && message);                                                                                                                                             \
	}                                                                                                                                                                         \
	while (false)
