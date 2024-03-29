#pragma once

#include "engine/light.h"
#include "engine/renderer/editor_camera.h"

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
		void OnKeyPressedEvent(const KeyPressedEvent& e);

		void RenderTargetResize(const glm::vec2& size);

		ShaderLibrary shader_library_;
		TextureLibrary texture_library_;
		rhi::TextureHandle texture_;

		rhi::TextureHandle rt_color_texture_;
		rhi::TextureHandle rt_color2_texture_;
		rhi::TextureHandle rt_depth_texture_;
		rhi::RenderTarget render_target_;

		glm::vec2 viewport_size_;
		
		Ref<Scene> active_secne_;
		SceneHierarchyPanel scene_hierarchy_panel_;
		PropertyPanel property_panel_;

		int guizmo_type_;

		EditorCamera editor_camera_;
	};
}