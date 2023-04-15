#include "particle_system.h"
#include "random.h"

using namespace light;

ParticleSystem::ParticleSystem()
{
	particle_pool_.resize(kParticlePoolSize);
}

void ParticleSystem::OnUpdate(light::Timestep ts)
{
	for (auto& particle : particle_pool_)
	{
		if (!particle.active)
		{
			continue;
		}

		particle.life_remaining -= ts;
		
		if (particle.life_remaining <= 0.f)
		{
			particle.active = false;
			continue;
		}
		
		particle.position += particle.velocity * (float)ts;
		particle.rotation += 10 * ts;
	}
}

void ParticleSystem::OnRender(rhi::CommandList* command_list)
{
	for (auto& particle : particle_pool_)
	{
		if (particle.rotation == 0.0f)
		{
			float progress = particle.life_remaining / particle.life_time;
			float size = particle.size_end * (1 - progress) + particle.size_begin * progress;
			glm::vec4 color = particle.color_end * (1 - progress) + particle.color_begin * progress;
			Renderer2D::DrawQuad(command_list, particle.position, glm::vec2(size), color);
		}
		else
		{
			float progress = particle.life_remaining / particle.life_time;
			float size = particle.size_end * (1 - progress) + particle.size_begin * progress;
			glm::vec4 color = particle.color_end * (1 - progress) + particle.color_begin * progress;
			Renderer2D::DrawRotationQuad(command_list, particle.position,particle.rotation,glm::vec2(size), color);
		}
	}
}

void ParticleSystem::Emit(const ParticleProps& particle_props)
{
	Particle& particle = particle_pool_[particle_index_];
	particle.active = true;
	particle.position = particle_props.position;
	particle.rotation = 0.0f;
	particle.velocity.x = particle_props.velocity.x + Random::Float() * particle_props.velocity_variation.x;
	particle.velocity.y = particle_props.velocity.y + Random::Float() * particle_props.velocity_variation.y;
	particle.color_begin = particle_props.color_begin;
	particle.color_end = particle_props.color_end;
	particle.size_begin = particle_props.size_begin + Random::Float() * particle_props.size_variation;
	particle.size_end = particle_props.size_end;
	particle.life_time = particle_props.life_time;
	particle.life_remaining = particle.life_time;
	
	particle_index_ = ++particle_index_ % particle_pool_.size();
}
