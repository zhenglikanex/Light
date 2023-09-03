#pragma once

#include <thread>
#include <mutex>

namespace light
{
	class RenderThread
	{
	public:
		enum class Status
		{
			kIdle,
			kBusy,
		};

		using RenderLoopFunc = void(*)(RenderThread*);

		RenderThread();

		void SetStatus(Status status);

		void Wait(Status status);

		void Join();
	private:
		Status status_ = Status::kIdle;
		std::thread thread_;
		std::condition_variable condition_variable_;
		std::mutex mutex_;
	};
}