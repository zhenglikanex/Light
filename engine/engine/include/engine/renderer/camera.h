#pragma once

#include "glm/glm.hpp"

namespace light
{
	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projection);
		virtual ~Camera() = default;

		void SetProjection(const glm::mat4& projection) { projection_ = projection; }
		const glm::mat4& GetProjection() const { return projection_; }

	private:
		glm::mat4 projection_;
	};

	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right,float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		void SetPosition(const glm::vec3& position) { position_ = position; RecalculateViewMatrix(); }
		const glm::vec3& GetPosition() const { return position_; }

		void SetRotation(float rotation) { rotation_ = rotation; RecalculateViewMatrix(); }
		float GetRotation() const { return rotation_; }

		const glm::mat4& GetProjectionMatrix() const { return projection_matrix_; }
		const glm::mat4& GetViewMatrix() const { return view_matrix_; }
		const glm::mat4& GetViewProjectionMatrix() const { return view_projection_matrx_; }
	private:
		void RecalculateViewMatrix();

		glm::mat4 projection_matrix_;
		glm::mat4 view_matrix_;
		glm::mat4 view_projection_matrx_;

		glm::vec3 position_;
		float rotation_ = 0.0f;
	};
}