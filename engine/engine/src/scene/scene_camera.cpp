#include "engine/scene/scene_camera.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace light
{
	SceneCamera::SceneCamera()
	{
		RecalculateProjection();
	}

	void light::SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
	{
		aspect_ratio_ = (float)width / (float)height;
		RecalculateProjection();
	}

	void SceneCamera::SetPerspective(float fov, float near, float far)
	{
		perspective_vertical_fov_ = fov;
		perspective_near_clip_ = near;
		perspective_far_clip_ = far;
		RecalculateProjection();
	}

	void SceneCamera::SetOrthographic(float size, float near, float far)
	{
		orthographic_size_ = size;
		orthographic_near_clip_ = near;
		orthographic_far_clip_ = far;

		RecalculateProjection();
	}

	void light::SceneCamera::SetProjectionType(ProjectionType type)
	{

	}

	void SceneCamera::RecalculateProjection()
	{
		float left = -orthographic_size_ * 0.5f * aspect_ratio_;
		float right = orthographic_size_ * 0.5f * aspect_ratio_;
		float top = orthographic_size_ * 0.5f;
		float bottom = -orthographic_size_ * 0.5f;

		SetProjection(glm::orthoLH_ZO(left, right, bottom, top, orthographic_near_clip_, orthographic_far_clip_));
	}
}