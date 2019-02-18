
#include"PerfTimer.h"

namespace Prizm
{
	PerfTimer::PerfTimer() : is_stopped_(false){}
	PerfTimer::~PerfTimer() = default;

	float PerfTimer::Tick()
	{
		if (is_stopped_)
		{
			dt_ = Duration::zero();
			return dt_.count();
		}

		curr_time_ = std::chrono::system_clock::now();
		dt_ = curr_time_ - prev_time_;

		prev_time_ = curr_time_;
		dt_ = dt_.count() < 0.0f ? dt_.zero() : dt_;

		return dt_.count();
	}

	void PerfTimer::Start()
	{
		if (is_stopped_)
		{
			paused_time_ = start_time_ - stop_time_;
			prev_time_ = std::chrono::system_clock::now();
			is_stopped_ = false;
		}
		Tick();
	}

	void PerfTimer::Stop()
	{
		Tick();
		if (!is_stopped_)
		{
			stop_time_ = std::chrono::system_clock::now();
			is_stopped_ = true;
		}
	}

	float PerfTimer::DeltaTime() const
	{
		return dt_.count();
	}

	float PerfTimer::TotalTime() const
	{
		Duration total_time = Duration::zero();

		if (is_stopped_) total_time = (stop_time_ - base_time_) - paused_time_;
		else total_time = (curr_time_ - base_time_) - paused_time_;

		return total_time.count();
	}

	void PerfTimer::Reset()
	{
		base_time_ = prev_time_ = std::chrono::system_clock::now();
		is_stopped_ = true;
		dt_ = Duration::zero();
	}
}
