#pragma once

namespace light
{
	class Timestep
	{
	public:
		explicit Timestep(float seconds);

		operator float() const { return seconds_; }

		float GetSeconds() const { return seconds_; }
		float GetMilliseconds() const { return seconds_ * 1000.f; }
	private:
		float seconds_;
	};
}