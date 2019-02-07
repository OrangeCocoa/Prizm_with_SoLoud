#pragma once

#include<string>

namespace Prizm
{
	namespace Log
	{
		enum LogMode
		{
			NONE = 0,
			CONSOLE,
			FILE,
			CONSOLE_AND_FILE,
		};

		void DLL_EXPORT Initialize(LogMode, std::string&);

		void DLL_EXPORT Finalize(void);

		void DLL_EXPORT Error(const std::string&);

		template<class... Args>
		void Error(const std::string& format, Args&&... args)
		{
			char msg[128];
			sprintf_s(msg, format, args...);
			Error(std::string(msg));
		}

		void DLL_EXPORT Warning(const std::string&);

		template<class... Args>
		void Warning(const std::string& format, Args&&... args)
		{
			char msg[128];
			sprintf_s(msg, format, args...);
			Warning(std::string(msg));
		}

		void DLL_EXPORT Info(const std::string&);
		
		template<class... Args>
		void Info(const std::string& format, Args&&... args)
		{
			char msg[128];
			sprintf_s(msg, format, args...);
			Info(std::string(msg));
		}

		void DLL_EXPORT InitConsole(void);

		void DLL_EXPORT InitFile(std::string&);
	};
}