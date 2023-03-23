#include "engine/renderer/camera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace light
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		projection_matrix_ = glm::orthoLH(left, right, bottom, top, 0.0f, 1.0f);
		view_matrix_ = glm::identity<glm::mat4>();
		position_ = glm::vec3(0);

		RecalculateViewMatrix();
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::identity<glm::mat4>(), position_) * glm::rotate(glm::identity<glm::mat4>(), rotation_, glm::vec3(0, 0, 1));
		view_matrix_ = glm::inverse(transform);
		view_projection_matrx_ = projection_matrix_ * view_matrix_;
	}

}