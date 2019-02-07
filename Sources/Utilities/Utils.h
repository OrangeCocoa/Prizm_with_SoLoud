#pragma once

#include<string>
#include<random>

namespace Prizm
{
	constexpr bool DLL_EXPORT succeeded(long hr) { return static_cast<int>(hr) >= 0; }
	constexpr bool DLL_EXPORT failed(long hr) { return static_cast<int>(hr) < 0; }

	// true, false, TRUE, FALSE
	/*inline constexpr bool succeeded(long hr) { return static_cast<int>(hr) > 0; }
	inline constexpr bool failed(long hr) { return static_cast<int>(hr) <= 0; }*/

	namespace StrUtils
	{
		std::string DLL_EXPORT UnicodeToAscii(wchar_t* pwstr);
		std::wstring DLL_EXPORT AsciiToUnicode(const char* pstr);

		namespace Time
		{
			std::string DLL_EXPORT GetCurrentTimeAsString(void);
			std::string DLL_EXPORT GetCurrentTimeAsStringWithBrackets(void);
		};
	}

	namespace DirectoryUtils
	{
		enum FolderType
		{
			PROGRAM_FILE,	//	%ProgramFiles% (%SystemDrive%\Program Files)
			APPDATA,		//	%APPDATA% (%USERPROFILE%\AppData\Roaming)
			LOCAL_APPDATA,	//	%LOCALAPPDATA% (%USERPROFILE%\AppData\Local)
			USER_PROFILE,	//	%USERPROFILE% (%SystemDrive%\Users\%USERNAME%) 
			DOCUMENTS,		//	%USERPROFILE%\Documents
		};

		std::string DLL_EXPORT GetSpecialFolderPath(FolderType);
	}

	namespace Utils
	{
		float DLL_EXPORT RandF(float low, float high);
		int	DLL_EXPORT RandI(int low, int high);
		size_t DLL_EXPORT RandU(size_t low, size_t high);

		template<class _T>
		_T RandomRange(_T low, _T high)
		{
			if (low > high)
			{// swap params
				_T tmp = low;
				low = high;
				high = tmp;
			}

			thread_local std::mt19937_64 engine(std::random_device{}());

			std::uniform_real_distribution<_T> distribution(low, high);

			return distribution(engine);
		}
	}
}
