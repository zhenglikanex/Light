#pragma once

#include "engine/light.h"

#include "particle_system.h"

namespace light
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(const Timestep& ts) override;
		void OnImGuiRender(const Timestep& ts) override;

		void OnEvent(const Event& e) override;
	private:
		ShaderLibrary shader_library_;
		TextureLibrary texture_library_;
		OrthographicCameraController camera_controller_;
		rhi::TextureHandle texture_;
		rhi::TextureHandle rt_color_texture_;
		rhi::TextureHandle rt_depth_texture_;

		ParticleSystem particle_system_;
		rhi::RenderTarget render_target_;
	};
}