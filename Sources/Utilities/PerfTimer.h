#pragma once

#include<memory>
#include<chrono>

namespace Prizm
{
	using Time = std::chrono::time_point<std::chrono::system_clock>;
	using Duration = std::chrono::duration<float>;

	class PerfTimer
	{
	private:
		Time		base_time_,
					prev_time_,
					curr_time_,
					start_time_,
					stop_time_;
		Duration	paused_time_,
					dt_;
		bool		is_stopped_;

	public:
		DLL_EXPORT PerfTimer();
		DLL_EXPORT ~PerfTimer();

		float DLL_EXPORT Tick();
		void DLL_EXPORT Start();
		void DLL_EXPORT Stop();
		float DLL_EXPORT DeltaTime() const;
		float DLL_EXPORT TotalTime() const;
		void DLL_EXPORT Reset();
	};
}