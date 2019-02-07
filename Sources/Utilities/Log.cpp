
#include<fstream>
#include<iostream>

#include<Windows.h>
#include<io.h>
#include<fcntl.h>

#define DLL_EXPORT __declspec(dllexport)

#include"Log.h"
#include"Utils.h"

namespace Prizm
{
	namespace Log
	{
		std::ofstream out_file_;
		LogMode current_mode_;
	}

	void Log::Initialize(LogMode mode, std::string& current_dir)
	{
		switch (mode)
		{
		case LogMode::NONE: 
			return;

		case LogMode::CONSOLE:
			InitConsole();
			break;

		case LogMode::FILE:
			InitFile(current_dir);
			break;

		case LogMode::CONSOLE_AND_FILE:
			InitConsole();
			InitFile(current_dir);
			break;

		default:
			break;
		}

		current_mode_ = mode;
	}

	void Log::Finalize(void)
	{
		std::string msg = StrUtils::Time::GetCurrentTimeAsStringWithBrackets() + "[Log] Finalize()";
		if (out_file_.is_open())
		{
			out_file_ << msg;
			out_file_.close();
		}
		std::cout << msg;
		OutputDebugStringA(msg.c_str());

		if (current_mode_ == CONSOLE || current_mode_ == CONSOLE_AND_FILE)
		{
			FreeConsole();
		}
	}

	void Log::Error(const std::string& s)
	{
		std::string err = StrUtils::Time::GetCurrentTimeAsStringWithBrackets() + "[ERROR]: " + s + "\n";

		OutputDebugStringA(err.c_str());	// vs
		
		if (out_file_.is_open())
			out_file_ << err;				// file
		
		std::cout << err;					// console
	}

	void Log::Warning(const std::string& s)
	{
		std::string warn = StrUtils::Time::GetCurrentTimeAsStringWithBrackets() + "[WARNING]: " + s + "\n";

		OutputDebugStringA(warn.c_str());	// vs

		if (out_file_.is_open())
			out_file_ << warn;				// file

		std::cout << warn;					// console
	}

	void Log::Info(const std::string& s)
	{
		std::string info = StrUtils::Time::GetCurrentTimeAsStringWithBrackets() + "[INFO]: " + s + "\n";

		OutputDebugStringA(info.c_str());	// vs

		if (out_file_.is_open())
			out_file_ << info;				// file

		std::cout << info;					// console
	}

	void Log::InitConsole(void)
	{
		// src: https://stackoverflow.com/a/46050762/2034041

		AllocConsole();

		// Get STDOUT handle
		HANDLE console_output = GetStdHandle(STD_OUTPUT_HANDLE);
		int system_output = _open_osfhandle(intptr_t(console_output), _O_TEXT);
		std::FILE *output_handle = _fdopen(system_output, "w");

		// Get STDERR handle
		HANDLE console_error = GetStdHandle(STD_ERROR_HANDLE);
		int system_error = _open_osfhandle(intptr_t(console_error), _O_TEXT);
		std::FILE *error_handle = _fdopen(system_error, "w");

		// Get STDIN handle
		HANDLE console_input = GetStdHandle(STD_INPUT_HANDLE);
		int system_input = _open_osfhandle(intptr_t(console_input), _O_TEXT);
		std::FILE *input_handle = _fdopen(system_input, "r");

		//make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
		std::ios::sync_with_stdio(true);

		// Redirect the CRT standard input, output, and error handles to the console
		freopen_s(&input_handle, "CONIN$", "r", stdin);
		freopen_s(&output_handle, "CONOUT$", "w", stdout);
		freopen_s(&error_handle, "CONOUT$", "w", stderr);

		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
	}

	void Log::InitFile(std::string& current_dir)
	{
		const std::string log_directory = current_dir + "\\Logs";

		std::string err_msg = "";

		if (CreateDirectoryA(current_dir.c_str(), nullptr))
		{
			if (CreateDirectoryA(log_directory.c_str(), nullptr))
			{
				std::string file_name = StrUtils::Time::GetCurrentTimeAsString() + "_PrizmEngine_Log.txt";

				out_file_.open(log_directory + "//" + file_name);

				if (out_file_)
				{
					std::string msg = StrUtils::Time::GetCurrentTimeAsStringWithBrackets() + "[Log] " + "Log Initialize Done.";
					out_file_ << msg;
					std::cout << msg << std::endl;
				}
				else
				{
					err_msg = "Cannot open log file " + file_name;
				}
			}
			else
			{
				err_msg = "Failed to create directory " + log_directory;
			}
		}
		else
		{
			err_msg = "Failed to create directory " + current_dir;
		}

		if (!err_msg.empty())
		{
			MessageBoxA(NULL, err_msg.c_str(), "PrizmEngine: Error Initializing Logging", MB_OK);
		}
	}
}