
#include"PerfTimer.h"

namespace Prizm
{
	PerfTimer::PerfTimer() : _is_stopped(false){}
	PerfTimer::~PerfTimer() = default;

	float PerfTimer::Tick()
	{
		if (_is_stopped)
		{
			_dt = Duration::zero();
			return _dt.count();
		}

		_curr_time = std::chrono::system_clock::now();
		_dt = _curr_time - _prev_time;

		_prev_time = _curr_time;
		_dt = _dt.count() < 0.0f ? _dt.zero() : _dt;

		return _dt.count();
	}

	void PerfTimer::Start()
	{
		if (_is_stopped)
		{
			_paused_time = _start_time - _stop_time;
			_prev_time = std::chrono::system_clock::now();
			_is_stopped = false;
		}
		Tick();
	}

	void PerfTimer::Stop()
	{
		Tick();
		if (!_is_stopped)
		{
			_stop_time = std::chrono::system_clock::now();
			_is_stopped = true;
		}
	}

	float PerfTimer::DeltaTime() const
	{
		return _dt.count();
	}

	float PerfTimer::TotalTime() const
	{
		Duration total_time = Duration::zero();

		if (_is_stopped) total_time = (_stop_time - _base_time) - _paused_time;
		else total_time = (_curr_time - _base_time) - _paused_time;

		return total_time.count();
	}

	void PerfTimer::Reset()
	{
		_base_time = _prev_time = std::chrono::system_clock::now();
		_is_stopped = true;
		_dt = Duration::zero();
	}
}
