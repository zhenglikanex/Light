#pragma once

#include "uuid/uuid.h"

namespace light
{
	using UUID = uuids::uuid;

	namespace uuid
	{
		inline std::mt19937 GetUUIDGenerator()
		{
			std::random_device rd;
			auto seed_data = std::array<int, std::mt19937::state_size> {};
			std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
			std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
			std::mt19937 generator(seq);
			return generator;
		}

		inline UUID Gen()
		{
			static std::mt19937 generator = GetUUIDGenerator();
			static uuids::uuid_random_generator gen{ generator };
			return gen();
		}

		inline std::optional<UUID> FromString(std::string_view value)
		{
			return uuids::uuid::from_string(value);
		}

		inline std::string ToString(const UUID& uuid)
		{
			return uuids::to_string(uuid);
		}
	}
	
}