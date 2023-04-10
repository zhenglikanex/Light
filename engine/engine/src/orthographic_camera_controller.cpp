#include "engine/core/orthographic_camera_controller.h"

#include "engine/input/input.h"

namespace light
{
	OrthographicCameraController::OrthographicCameraController(float aspect_ratio, float rotation)
		: aspect_ratio_(aspect_ratio)
		, zoom_level_(1.0f)
		, rotation_(rotation)
		, speed_(10)
		, rotation_speed_(10)
		, camera_(- aspect_ratio_ * zoom_level_, aspect_ratio_* zoom_level_, -zoom_level_, zoom_level_)
	{
	}

	void OrthographicCameraController::OnUpdate(const Timestep& ts)
	{
		float speed = speed_ * ts;
		glm::vec3 position = camera_.GetPosition();

		if (Input::IsKeyPressed(Input::Key::KEY_W))
		{
			position += glm::vec3(0,speed, 0);
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_S))
		{
			position += glm::vec3(0, -speed, 0);
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_A))
		{
			position += glm::vec3(-speed, 0, 0);
		}
		else if (Input::IsKeyPressed(Input::Key::KEY_D))
		{
			position += glm::vec3(speed, 0, 0);
		}
		
		camera_.SetPosition(position);

		if (rotation_)
		{
			float rotation_speed = rotation_speed_ * ts;
			if (Input::IsKeyPressed(Input::Key::KEY_A))
			{
				camera_.SetRotation(camera_.GetRotation() + rotation_speed);
			}
			else if (Input::IsKeyPressed(Input::Key::KEY_D))
			{
				camera_.SetRotation(camera_.GetRotation() - rotation_speed);
			}
		}
		
	}

	void OrthographicCameraController::OnEvent(const Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&OrthographicCameraController::OnMouseScrolledEvent, this, std::placeholders::_1));
		dispatcher.Dispatch<WindowResizedEvent>(std::bind(&OrthographicCameraController::OnWindowResizedEvent, this, std::placeholders::_1));
		
	}
	void OrthographicCameraController::OnMouseScrolledEvent(const MouseScrolledEvent& e)
	{
		zoom_level_ += e.y * 0.25;
		zoom_level_ = std::max(0.250f, zoom_level_);
		camera_.SetProjection(-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_, -zoom_level_, zoom_level_);
	}

	void OrthographicCameraController::OnWindowResizedEvent(const WindowResizedEvent& e)
	{
		aspect_ratio_ = static_cast<float>(e.width) / static_cast<float>(e.height);
		//camera_.SetProjection(-aspect_ratio_ * zoom_level_, aspect_ratio_ * zoom_level_, -zoom_level_, zoom_level_);
	}
}