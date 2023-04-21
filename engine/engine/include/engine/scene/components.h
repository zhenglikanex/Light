#pragma once
#include "light_pch.h"

#include "engine/core/reflection.h"
#include "engine/scene/scene_camera.h"
#include "engine/scene/script.h"

#include "glm/glm.hpp"



namespace ns_3d
{
	class node
	{
	public:
		node(std::string name, node* parent = nullptr);
		virtual ~node() { }
		void set_name(const std::string& name) { m_name = name; }
		const std::string& get_name() const { return m_name; }
		std::vector<node*> get_children() const { return m_children; }
		void set_visible(bool visible, bool cascade = true) {  }
		virtual void render() {  }
	private:
		node* m_parent;
		std::string         m_name;
		std::vector<node*>  m_children;
		RTTR_ENABLE()
	};
}

namespace light
{
	STRUCT() TagComponent
	{
		std::string tag;

		TagComponent(std::string & name) { tag = name; }
		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(std::string_view tag)
			: tag(tag)
		{

		}
	};

	STRUCT() TransformComponent
	{
		glm::mat4 transform = glm::mat4(1.0f);

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::mat4& transform)
			: transform(transform)
		{
		}

		operator glm::mat4&() { return transform; }
	};

	STRUCT() SpriteRendererComponent
	{
		glm::vec4 color = { 0.0f,0.0f,0.0f,1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& other) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: color(color)
		{
		}
	};

	STRUCT() CameraComponent
	{
		SceneCamera camera;
		bool primary = true;
		bool aspect_ratio_fixed = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
	};

	STRUCT() NativeScriptComponent
	{
		Script* script_instance = nullptr;

		using ScriptInstanceConstructor = Script * (*)();
		using ScriptInstanceDestroy = void(*)(Script*);

		ScriptInstanceConstructor script_instance_constructor = nullptr;	
		ScriptInstanceDestroy script_instance_destroy = nullptr;

		NativeScriptComponent() = default;
		NativeScriptComponent(const NativeScriptComponent& other) = delete;
		NativeScriptComponent(NativeScriptComponent&& other) = default;
		NativeScriptComponent& operator=(const NativeScriptComponent& other) = delete;
		NativeScriptComponent& operator=(NativeScriptComponent&& other) = default;

		template<class T>
		void Bind()
		{
			script_instance_constructor = []() { return (Script*)new T(); };
			script_instance_destroy = [](Script* instance) { delete instance; };
		}
	};
}