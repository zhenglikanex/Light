#pragma once

#include "engine/light.h"

#include "panel/scene_hierarchy_panel.h"
#include "panel/property_panel.h"

namespace light::editor
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer();

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(const Timestep& ts) override;
		void OnImGuiRender(const Timestep& ts) override;

		void OnEvent(Event& e) override;
	private:
		void RenderTargetResize(const glm::vec2& size);

		ShaderLibrary shader_library_;
		TextureLibrary texture_library_;
		rhi::TextureHandle texture_;
		rhi::TextureHandle rt_color_texture_;
		rhi::TextureHandle rt_depth_texture_;
		rhi::RenderTarget render_target_;
		glm::vec2 viewport_size_;
		
		Ref<Scene> active_secne_;
		SceneHierarchyPanel scene_hierarchy_panel_;
		PropertyPanel property_panel_;

		Entity quad_entity_;
		Entity camera_entity_;
	};
}