#pragma once
#include "light_pch.h"

#include "engine/reflection/meta.h"
#include "engine/scene/scene_camera.h"
#include "engine/scene/script.h"
#include "engine/renderer/mesh.h"
#include "engine/renderer/shader_library.h"
#include "engine/serializer/material_serializer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"



namespace light
{
	struct META() Component
	{
		
	};

	struct META() TagComponent : public Component
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent& other) = default;
		TagComponent(std::string_view tag)
			: tag(tag)
		{

		}
	};

	struct META() TransformComponent : public Component
	{
		glm::vec3 position = glm::vec3(0);
		glm::vec3 rotation = glm::vec3(0);
		glm::vec3 scale = glm::vec3(1);

		TransformComponent() = default;
		TransformComponent(const TransformComponent& other) = default;
		TransformComponent(const glm::vec3& position,const glm::vec3& rotation,glm::vec3& scale)
			: position(position), rotation(rotation), scale(scale)
		{
			
		}

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotate = glm::toMat4(glm::quat(glm::radians(rotation)));
			return glm::translate(glm::mat4(1.0f), position) * rotate * glm::scale(glm::mat4(1.0f), scale);
		}
		
	};

	struct META() SpriteRendererComponent : public Component
	{
		glm::vec4 color = { 0.0f,0.0f,0.0f,1.0f };

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent& other) = default;
		SpriteRendererComponent(const glm::vec4& color)
			: color(color)
		{
		}
	};

	struct META() CameraComponent : public Component
	{
		SceneCamera camera;
		bool primary = true;
		bool aspect_ratio_fixed = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent& other) = default;
	};

	struct META() MeshComponent : public Component
	{
		NOT_PROPERTY()
		Ref<Mesh> mesh;

		MeshComponent() = default;
		MeshComponent(const MeshComponent& other) = default;
		MeshComponent(std::string_view mesh_file)
			: mesh(MakeRef<Mesh>(mesh_file))
		{

		}

		MeshComponent(const Ref<Mesh>& mesh)
			: mesh(mesh)
		{
			Ref<Shader> shader = ShaderLibrary::Get().Get("simplepbr");

			auto material = MakeRef<Material>(shader);
			
			material->Set("cbAlbedoColor", glm::vec3(1.0f,0.0f,0.0f));
			material->Set("cbMetalness",0.5f);
			material->Set("cbRoughness", 0.3f);

			for (uint32_t i = 0; i < mesh->GetNumSubMesh(); ++i)
			{
				mesh->SetMaterial(i,material);
			}
		}

		void ImGuiDrawProperty();

		void SerializeText(YAML::Emitter* out);

		void DeserializeText(YAML::Node node);
	};

	struct META() LightComponent : public Component
	{
		glm::vec3 color = glm::vec3(1);

		LightComponent() = default;
		LightComponent(const LightComponent& other) = default;
		LightComponent(const glm::vec3 & color)
			: color(color)
		{
		}
	};

	struct NativeScriptComponent : public Component
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