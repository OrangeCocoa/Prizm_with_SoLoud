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
		Time     _base_time,
		         _prev_time,
	             _curr_time,
		         _start_time,
		         _stop_time;
		Duration _paused_time,
	             _dt;
		bool     _is_stopped;

		float Tick();

	public:
		PerfTimer();
		~PerfTimer();

		void Start();
		void Stop();
		float DeltaTime() const;
		// not include stopped time
		float TotalTime() const;
		void Reset();
	};
}