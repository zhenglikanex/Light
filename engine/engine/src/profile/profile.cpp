#include "engine/profile/profile.h"

namespace light
{
	std::mutex Profile::mutex_;
	uint32_t Profile::current_index_ = 0;
	std::vector<ProfileResult> Profile::profile_results_[2];

	void Profile::PushProfileResult(ProfileResult&& result)
	{
		std::unique_lock<std::mutex> lock(mutex_);
		profile_results_[current_index_].emplace_back(std::move(result));
	}

	const std::vector<ProfileResult>& Profile::GetProfileResults()
	{
		std::unique_lock<std::mutex> lock(mutex_);

		uint32_t last_index = current_index_ == 0 ? 1 : 0;
		return profile_results_[last_index];
	}

	void Profile::ClearProfileResult()
	{
		std::unique_lock<std::mutex> lock(mutex_);
		current_index_ = current_index_ == 0 ? 1 : 0;
		profile_results_[current_index_].clear();
	}
}

