#pragma once

#include "engine/event/event.h"
#include "engine/event/mouse_event.h"
#include "engine/event/windows_event.h"
#include "engine/core/timestep.h"
#include "engine/renderer/camera.h"

namespace light
{
	class OrthographicCameraController
	{
	public:
		explicit OrthographicCameraController(float aspect_ratio,float rotation);

		void OnUpdate(const Timestep& ts);
		void OnEvent(const Event& e);

		const OrthographicCamera& GetCamera() const { return camera_; }
	private:

		void OnMouseScrolledEvent(const MouseScrolledEvent& e);
		void OnWindowResizedEvent(const WindowResizedEvent& e);

		float aspect_ratio_;
		float zoom_level_;
		float rotation_;
		float speed_;
		float rotation_speed_;
		OrthographicCamera camera_;
	};
}