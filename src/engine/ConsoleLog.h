#pragma once
#include <assert.h>
#include <iomanip>
#include <iostream>
#include <vector>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <format>

class CustomStreamBuffer : public std::streambuf
{
public:
	virtual int_type overflow(int_type c = traits_type::eof())
	{
		if (c != traits_type::eof())
		{
			// Convert the character to a string and output to the debug window
			std::string output(1, static_cast<char>(c));
#ifdef _WIN32
			OutputDebugStringA(output.c_str());
#else
			// Output to another platform's debug stream
#endif
		}

		return c;
	}
};

class CustomStream : public std::ostream
{
public:
	CustomStream() : std::ostream(&buffer) {}

private:
	CustomStreamBuffer buffer;
};

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
	// Compile-time checked formatting (preferred when using string literals)
	template<typename... Args>
	static void logFormatted(ConsoleColor color, std::format_string<Args...> fmt, Args&&... args)
	{
		set(color);
		std::cout << std::format(fmt, std::forward<Args>(args)...) << std::endl;
		reset();
	}

	// Runtime-formattable fallback when the format string is not a compile-time literal
	template<typename... Args>
	static void logFormatted(ConsoleColor color, std::string_view fmt, Args&&... args)
	{
		set(color);
		try
		{
			std::cout << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
		}
		catch (const std::format_error& e)
		{
			// Fallback: print the raw format and the error
			std::cout << "Format error: " << e.what() << " - format: " << std::string(fmt) << std::endl;
		}
		reset();
	}

	// Function for debugging with file and line information (INFO)
	template<typename... Args>
	static void internalInfo(const char* file, int line, std::format_string<Args...> fmt, Args&&... args)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Cyan);
		std::cout << "INFO";
		ConsoleLog::reset();

		std::cout << " - " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
	}

	// Runtime fallback when format string is not a literal
	template<typename... Args>
	static void internalInfo(const char* file, int line, std::string_view fmt, Args&&... args)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Cyan);
		std::cout << "INFO";
		ConsoleLog::reset();

		try
		{
			std::cout << " - " << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
		}
		catch (const std::format_error& e)
		{
			std::cout << "Format error: " << e.what() << " - format: " << std::string(fmt) << std::endl;
		}
	}

	// Function for debugging with file and line information (WARN)
	template<typename... Args>
	static void internalWarn(const char* file, int line, std::format_string<Args...> fmt, Args&&... args)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Yellow);
		std::cout << "WARN";
		ConsoleLog::reset();

		std::cout << " - " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
	}

	// Runtime fallback when format string is not a literal
	template<typename... Args>
	static void internalWarn(const char* file, int line, std::string_view fmt, Args&&... args)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Yellow);
		std::cout << "WARN";
		ConsoleLog::reset();

		try
		{
			std::cout << " - " << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
		}
		catch (const std::format_error& e)
		{
			std::cout << "Format error: " << e.what() << " - format: " << std::string(fmt) << std::endl;
		}
	}

	// Function for debugging with file and line information (ERROR)
	template<typename... Args>
	static void internalError(const char* file, int line, std::format_string<Args...> fmt, Args&&... args)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Red);
		std::cout << "ERROR";
		ConsoleLog::reset();

		std::cout << " - " << std::format(fmt, std::forward<Args>(args)...) << std::endl;
	}

	// Runtime fallback when format string is not a literal
	template<typename... Args>
	static void internalError(const char* file, int line, std::string_view fmt, Args&&... args)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Red);
		std::cout << "ERROR";
		ConsoleLog::reset();

		try
		{
			std::cout << " - " << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
		}
		catch (const std::format_error& e)
		{
			std::cout << "Format error: " << e.what() << " - format: " << std::string(fmt) << std::endl;
		}
	}

	// Debug macro for convenience
	// Raw logging helpers: accept a pre-formatted message to avoid overload ambiguity
	static void internalInfoRaw(const char* file, int line, const std::string& message)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Cyan);
		std::cout << "INFO";
		ConsoleLog::reset();

		std::cout << " - " << message << std::endl;
	}

	static void internalWarnRaw(const char* file, int line, const std::string& message)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Yellow);
		std::cout << "WARN";
		ConsoleLog::reset();

		std::cout << " - " << message << std::endl;
	}

	static void internalErrorRaw(const char* file, int line, const std::string& message)
	{
		std::cout << std::format("{}({}) : ", file, line);
		ConsoleLog::set(ConsoleColor::Red);
		std::cout << "ERROR";
		ConsoleLog::reset();

		std::cout << " - " << message << std::endl;
	}
};

// Convenience macros that format using std::format (compile-time checked for literals)
#define SLIME_INFO(...) ConsoleLog::internalInfoRaw(__FILE__, __LINE__, std::format(__VA_ARGS__))
#define SLIME_WARN(...) ConsoleLog::internalWarnRaw(__FILE__, __LINE__, std::format(__VA_ARGS__))

#define SLIME_ERROR(...)                                                                                                                                                      \
	do                                                                                                                                                                        \
	{                                                                                                                                                                         \
		std::string message = std::format(__VA_ARGS__);                                                                                                                       \
		ConsoleLog::internalErrorRaw(__FILE__, __LINE__, message);                                                                                                            \
		assert(false && message.c_str());                                                                                                                                     \
	}                                                                                                                                                                         \
	while (false)