#include "engine/renderer/render_thread.h"
#include "engine/renderer/renderer.h"
namespace light
{
	RenderThread::RenderThread()
	{
		thread_ = std::thread(&Renderer::RenderLoop, this);
	}

	void RenderThread::SetStatus(Status status)
	{
		std::lock_guard lock(mutex_);
		status_ = status;

		if(status_ == Status::kIdle)
		{
			condition_variable_.notify_one();
		}
	}

	void RenderThread::Wait(Status status)
	{
		std::unique_lock lock(mutex_);

		condition_variable_.wait(lock, [this,status]()
			{
				return status_ == status;
			});
	}

	void RenderThread::Join()
	{
		
		thread_.join();
	}
}
