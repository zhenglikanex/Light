#pragma once

#include "engine/light.h"
#include "engine/renderer/editor_camera.h"

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

	light::EditorCamera editor_camera_;
	light::Ref<light::Material> material_;
	light::Ref<light::Mesh> mesh_;
};