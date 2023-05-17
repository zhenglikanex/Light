#pragma once

#include "engine/core/timestep.h"

#include "glm/glm.hpp"

namespace light
{
	class EditorCamera
	{
	public:
		EditorCamera();

		void OnUpdate(Timestep ts);

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetFov(float fov);

		void SetPosition(const glm::vec3& position) { position_ = position; Recalculate(); }

		void SetRotation(const glm::vec3& rotation) { rotation_ = rotation; Recalculate(); }
		 
		glm::vec3 GetForward() const;

		glm::vec3 GetRight() const;

		glm::vec3 GetPosition() const { return position_; }

		const glm::mat4& GetProjectionMatrx() const { return projection_matrix_; }
		const glm::mat4& GetViewMatrix() const { return view_matrix_; }
		const glm::mat4& GetViewProjectionMatrix() const { return view_projection_matrix_; }
	private:
		void Recalculate();
		glm::mat4 GetTransform() const;

		float movement_speed_ = 20.0f;
		float rotate_speed_ = 0.2f;

		float fov_ = glm::radians(30.0);
		float far_clip_ = 1000.f;
		float near_clip_ = 0.1f;
		float aspect_ratio_ = 1.0f;

		glm::vec3 position_ = { 0.0f,0.0f,-10.0f };
		glm::vec3 rotation_ = glm::vec3(0);

		glm::mat4 projection_matrix_;
		glm::mat4 view_matrix_;
		glm::mat4 view_projection_matrix_;

		float last_mouse_x_ = 0;
		float last_mouse_y_ = 0;
	};
}