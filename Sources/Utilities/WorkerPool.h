#pragma once

#include<thread>
#include<mutex>
#include<deque>
#include<vector>
#include<condition_variable>
#include<cassert>

namespace Prizm
{
	template<typename _T>
	class TaskQueue
	{
	private:
		std::deque<_T> _deque;
		int _size;

	public:
		TaskQueue(int size) : _size(size) {}

		bool Push(const _T& task)
		{
			if (_size <= _deque.size())
				return false;

			_deque.emplace_back(task);
			return true;
		}

		bool Push(_T&& task)
		{
			if (_size <= _deque.size())
				return false;

			_deque.emplace_back(std::move(task));
			return true;
		}

		bool Pop(_T& task)
		{
			if (_deque.empty())
				return false;

			task = std::move(_deque.front());
			_deque.pop_front();
			return true;
		}

		bool Empty(void)
		{
			return _deque.empty();
		}
	};

	class WorkerPool
	{
	private:
		TaskQueue<std::function<void()>> _pool;
		bool _is_terminated;
		std::mutex _mutex;
		std::condition_variable _cv;
		std::vector<std::thread> _threads;

	public:
		WorkerPool(int thread_count, int queue_size)
			: _is_terminated(false)
			, _pool(queue_size)
		{
			for (int i = 0; i < thread_count; ++i)
			{
				_threads.emplace_back(_run);
			}
		}

		~WorkerPool(void)
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_is_terminated = true;

			_cv.notify_all();

			const int size = _threads.size();

			for (int i = 0; i < size; ++i)
			{
				_threads.at(i).join();
			}
		}

		bool Add(const std::function<void()>& task)
		{
			std::unique_lock<std::mutex> lock(_mutex);

			if (!_pool.Push(task)) return false;

			_cv.notify_all();

			return true;
		}

		bool Add(std::function<void()>&& task)
		{
			std::unique_lock<std::mutex> lock(_mutex);

			if (!_pool.Push(std::move(task))) return false;

			_cv.notify_all();

			return true;
		}

	private:
		std::function<void()> _run = [this]
		{
			while (true)
			{
				std::function<void()> func;

				std::unique_lock<std::mutex> lock(_mutex);

				while (_pool.Empty()) 
				{
					if (_is_terminated) return;

					_cv.wait(lock);
				}

				const bool result = _pool.Pop(func);
				assert(result);

				func();
			}
		};
	};
}