#pragma once
#include <vector>

#include "engine/light.h"

struct ParticleProps
{
	glm::vec2 position;
	glm::vec2 velocity, velocity_variation;
	glm::vec4 color_begin, color_end;
	float size_begin, size_end, size_variation;
	float life_time = 1.0f;
};

class ParticleSystem
{
public:
	ParticleSystem();

	void OnUpdate(light::Timestep ts);
	void OnRender(light::rhi::CommandList* command_list);
	void Emit(const ParticleProps& particle_props);
private:
	constexpr static uint32_t kParticlePoolSize = 1000;

	struct Particle
	{
		glm::vec2 position;
		float rotation;
		glm::vec2 velocity;
		glm::vec4 color_begin, color_end;
		float size_begin, size_end;
		float life_time;
		float life_remaining = 0.0f;

		bool active = false;
	};

	std::vector<Particle> particle_pool_;
	uint32_t particle_index_ = 0;
};