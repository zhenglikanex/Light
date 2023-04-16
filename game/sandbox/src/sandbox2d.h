#pragma once

#include "engine/light.h"

#include "particle_system.h"

class Sandbox2D : public light::Layer
{
public:
	Sandbox2D();

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(const light::Timestep& ts) override;
	void OnImGuiRender(const light::Timestep& ts) override;

	void OnEvent(light::Event& e) override;
private:
	light::ShaderLibrary shader_library_;
	light::TextureLibrary texture_library_;
	light::rhi::BufferHandle vertex_buffer_;
	light::rhi::BufferHandle index_buffer_;
	light::rhi::GraphicsPipelineHandle pso_;
	light::OrthographicCameraController camera_controller_;
	glm::vec3 color_;
	light::rhi::TextureHandle texture_;
	light::rhi::TextureHandle rt_color_texture_;
	light::rhi::TextureHandle rt_depth_texture_;
	
	ParticleSystem particle_system_;
	light::rhi::RenderTarget render_target_;
};