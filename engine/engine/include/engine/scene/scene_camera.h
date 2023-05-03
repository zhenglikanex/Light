#pragma once

#undef near
#undef far

#include "light_pch.h"

#include "engine/renderer/camera.h"
#include "engine/reflection/meta.h"
#include "engine/reflection/attribute.h"

namespace light
{
	//SceneCamera, 用于保存跟场景相关的数据，Camera对于Renderer来说只需要知道投影矩阵
	class META() SceneCamera : public Camera
	{
	public:
		FRIEND_META();

		enum class META() ProjectionType
		{
			kPerspective = 0,
			kOrthographic = 1
		};

		SceneCamera();

		void SetViewportSize(uint32_t width, uint32_t height);

		void SetPerspective(float fov, float near, float far);

		void SetOrthographic(float size, float near, float far);

		void SetPerspectiveVerticalFov(float perspective_vertical_fov);

		void SetPerspectiveNearClip(float perspective_near_clip);

		void SetPerspectiveFarClip(float perspective_far_clip);

		void SetProjectionType(ProjectionType type);

		ProjectionType GetProjectionType() const { return projection_type_; }

		float GetPerspectiveVerticalFov() const { return perspective_vertical_fov_; }

		float GetPerspectiveNearClip() const { return perspective_near_clip_; }

		float GetPerspectiveFarClip() const { return perspective_far_clip_; }

		float GetOrthographicSize() const { return orthographic_size_; }
	public:
		void RecalculateProjection();

		ProjectionType projection_type_ = ProjectionType::kOrthographic;

		PROPERTY(meta::attribute::EditorCondition{ "projection_type_", (uint64_t)SceneCamera::ProjectionType::kPerspective })
		float perspective_vertical_fov_ = glm::radians(45.0f);

		PROPERTY(meta::attribute::EditorCondition{ "projection_type_",(uint64_t)SceneCamera::ProjectionType::kPerspective })
		float perspective_near_clip_ = 0.1f;

		PROPERTY(meta::attribute::EditorCondition("projection_type_", (uint64_t)SceneCamera::ProjectionType::kPerspective))
		float perspective_far_clip_ = 1000.0f;

		PROPERTY(meta::attribute::EditorCondition( "projection_type_",(uint64_t)SceneCamera::ProjectionType::kOrthographic ))
		float orthographic_size_ = 10.0f;

		PROPERTY(meta::attribute::EditorCondition{ "projection_type_", (uint64_t)SceneCamera::ProjectionType::kOrthographic })
		float orthographic_near_clip_ = -1.0f;

		PROPERTY(meta::attribute::EditorCondition{ "projection_type_", (uint64_t)SceneCamera::ProjectionType::kOrthographic })
		float orthographic_far_clip_ = 1.0f;

		PROPERTY(meta::attribute::ShowInEditor(false))
		float aspect_ratio_ = 1.0f;
	};

	
}