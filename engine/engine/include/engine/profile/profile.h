#pragma once

#include <vector>
#include <string>
#include <mutex>

#include "engine/core/core.h"
#include "engine/profile/timer.h"

namespace light
{
	struct ProfileResult
	{
		std::string name;
		float time;
	};

	class Profile
	{
	public:
		static void PushProfileResult(ProfileResult&& result);

		static const std::vector<ProfileResult>& GetProfileResults();

		static void ClearProfileResult();
	private:
		static std::mutex mutex_;
		static uint32_t current_index_;
		static std::vector<ProfileResult> profile_results_[2];
	};

#ifdef PROFILE
	#define PROFILE_FUNCATION() Timer timer__FILE__##__LINE__(__FUNCSIG__,[](ProfileResult&& result) { Profile::PushProfileResult(std::move(result)); })
	#define PROFILE_SCOPE(name) Timer timer__FILE__##__LINE__(name,[](ProfileResult&& result) { Profile::PushProfileResult(std::move(result)); })
#else
	#define PROFILE_FUNCATION()
	#define PROFILE_SCOPE(name)
#endif
}