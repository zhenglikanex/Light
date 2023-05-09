#include "engine/renderer/editor_camera.h"
#include "engine/input/input.h"
#include "engine/log/log.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace light
{
	EditorCamera::EditorCamera()
	{
		Recalculate();
	}

	void EditorCamera::OnUpdate(Timestep ts)
	{
		if (Input::IsKeyPressed(Input::Key::KEY_W))
		{
			auto pos = GetForward()* movement_speed_* ts.GetSeconds();
			position_ += GetForward() * movement_speed_ * ts.GetSeconds();

			Recalculate();
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_S))
		{
			auto pos = GetForward() * movement_speed_ * ts.GetSeconds();
			position_ -= GetForward() * movement_speed_ * ts.GetSeconds();
			Recalculate();
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_A))
		{
			position_ -= GetRight() * movement_speed_ * ts.GetSeconds();
			Recalculate();
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_D))
		{
			position_ += GetRight() * movement_speed_ * ts.GetSeconds();
			Recalculate();
		}
		
		if (Input::IsMouseButtonPressed(Input::MouseButton::BUTTON_RIGHT))
		{
			if (last_mouse_x_ == 0)
			{
				last_mouse_x_ = Input::GetMouseX();
			}

			if (last_mouse_y_ == 0)
			{
				last_mouse_y_ = Input::GetMouseY();
			}

			rotation_.y += (Input::GetMouseX() - last_mouse_x_) * rotate_speed_ * ts.GetSeconds();
			last_mouse_x_ = Input::GetMouseX();

			rotation_.x += (Input::GetMouseY() - last_mouse_y_) * rotate_speed_ * ts.GetSeconds();
			last_mouse_y_ = Input::GetMouseY();

			Recalculate();
		}
		else 
		{
			last_mouse_x_ = 0;
			last_mouse_y_ = 0;
		}
	}

	void EditorCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		aspect_ratio_ = (float)width / (float)height;
		Recalculate();
	}

	void EditorCamera::SetFov(float fov)
	{
		fov_ = glm::radians(fov);
		Recalculate();
	}

	glm::vec3 EditorCamera::GetForward() const
	{
		glm::mat4 rotate = glm::toMat4(glm::quat(rotation_));

		glm::vec3 forward = rotate * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		
		return glm::normalize(forward);
	}

	glm::vec3 EditorCamera::GetRight() const
	{
		glm::mat4 rotate = glm::toMat4(glm::quat(rotation_));
		glm::vec3 right = rotate * glm::vec4(1, 0, 0.0f, 1.0f);
		return glm::normalize(right);
	}

	void EditorCamera::Recalculate()
	{
		projection_matrix_ = glm::perspectiveLH_ZO(fov_, aspect_ratio_, near_clip_, far_clip_);
		view_matrix_ = glm::inverse(GetTransform());
		view_projection_matrix_ = projection_matrix_ * view_matrix_;
	}

	glm::mat4 EditorCamera::GetTransform() const
	{
		glm::mat4 rotate = glm::toMat4(glm::quat(rotation_));
		return glm::translate(glm::mat4(1.0f), position_) * rotate;
	}
}