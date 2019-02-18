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