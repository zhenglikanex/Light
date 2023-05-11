#pragma once

#include "engine/light.h"
#include "engine/renderer/editor_camera.h"

struct PbrMaterial
{
	glm::vec3 albedo = { 1.0f,0.0f,0.0f };         // 基础颜色
	float metalness = 0.9;          // 金属度 (0-1)
	float roughness = 0.1;          // 粗糙度(0-1)
};

struct Light
{
	glm::vec3 direction = { 0.0f,-1.f,0.0f };
	glm::vec3 color = { 0.9f,0.9f,0.9f };
};

class Sandbox3D : public light::Layer
{
public:
	Sandbox3D();

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(const light::Timestep& ts) override;
	void OnImGuiRender(const light::Timestep& ts) override;

	void OnEvent(light::Event& e) override;
private:
	void CreateMesh(std::string_view fielname);

	light::ShaderLibrary shader_library_;
	light::Ref<light::Shader> shader_;
	light::EditorCamera editor_camera_;
	light::Ref<light::Material> material_;
	light::Ref<light::Mesh> mesh_;
	PbrMaterial pbr_material_;
	Light light_;
};