#include "light_pch.h"

#include "engine/scene/scene_camera.h"
#include "engine/scene/script.h"

#include "glm/glm.hpp"

namespace light
{
	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(std::string_view tag)
			: tag(tag)
		{

		}
	};

	struct TransformComponent
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

	struct SpriteRendererComponent
	{
		glm::vec4 color = { 0.0f,0.0f,0.0f,1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& other) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: color(color)
		{
		}
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool primary = true;
		bool aspect_ratio_fixed = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
	};

	struct NativeScriptComponent
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