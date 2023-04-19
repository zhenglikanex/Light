#pragma once

#undef near
#undef far

#include "light_pch.h"

#include "engine/renderer/camera.h"

namespace light
{
	//SceneCamera, 用于保存跟场景相关的数据，Camera对于Renderer来说只需要知道投影矩阵
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0,
			Orthographic = 1
		};

		SceneCamera();

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetPerspective(float fov, float near, float far);

		void SetOrthographic(float size, float near, float far);

		void SetProjectionType(ProjectionType type);

		ProjectionType GetProjectionType() const { return projection_type_; }

		float GetPerspectiveVerticalFOV() const { return perspective_vertical_fov_; }

		float GetPerspectiveNearClip() const { return perspective_near_clip_; }

		float GetPerspectiveFarClip() const { return perspective_far_clip_; }

		float GetOrthographicSize() const { return orthographic_size_; }
	private:
		void RecalculateProjection();

		ProjectionType projection_type_ = ProjectionType::Orthographic;
		float perspective_vertical_fov_ = glm::radians(45.0f);
		float perspective_near_clip_ = 0.1f;
		float perspective_far_clip_ = 1000.0f;
		float orthographic_size_ = 10.0f;
		float orthographic_near_clip_ = -1.0f;
		float orthographic_far_clip_ = 1.0f;
		float aspect_ratio_ = 1.0f;
	};
}