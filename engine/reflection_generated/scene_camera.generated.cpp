/* this file is auto-generated. do not edit! */
#include "engine/reflection/registry.h"
#include "reflection_generated.h"
#include"C:\Project\Light\engine\engine\include\engine\scene\scene_camera.h"
using namespace light;
using namespace meta;
namespace meta_generated { 
static void AutoRegisterTypes() { 
{
TypeData &data = Registry::Get().AddTypeData<light::SceneCamera::ProjectionType>("light::SceneCamera::ProjectionType");
data.AddEnum("kPerspective", static_cast<int64_t>(light::SceneCamera::ProjectionType::kPerspective));
data.AddEnum("kOrthographic", static_cast<int64_t>(light::SceneCamera::ProjectionType::kOrthographic));
}
{
TypeData &data = Registry::Get().AddTypeData<light::SceneCamera>("light::SceneCamera");
data.AddBaseType("light::Camera");
data.AddField<light::SceneCamera, glm::mat<4, 4, float>>("projection_", &light::SceneCamera::projection_);
data.AddField<light::SceneCamera, light::SceneCamera::ProjectionType>("projection_type_", &light::SceneCamera::projection_type_);
data.AddField<light::SceneCamera, float>("perspective_vertical_fov_", &light::SceneCamera::perspective_vertical_fov_,meta::attribute::EditorCondition{ "projection_type_", (uint64_t)SceneCamera::ProjectionType::kPerspective });
data.AddField<light::SceneCamera, float>("perspective_near_clip_", &light::SceneCamera::perspective_near_clip_,meta::attribute::EditorCondition{ "projection_type_",(uint64_t)SceneCamera::ProjectionType::kPerspective });
data.AddField<light::SceneCamera, float>("perspective_far_clip_", &light::SceneCamera::perspective_far_clip_,meta::attribute::EditorCondition("projection_type_", (uint64_t)SceneCamera::ProjectionType::kPerspective));
data.AddField<light::SceneCamera, float>("orthographic_size_", &light::SceneCamera::orthographic_size_,meta::attribute::EditorCondition( "projection_type_",(uint64_t)SceneCamera::ProjectionType::kOrthographic ));
data.AddField<light::SceneCamera, float>("orthographic_near_clip_", &light::SceneCamera::orthographic_near_clip_,meta::attribute::EditorCondition{ "projection_type_", (uint64_t)SceneCamera::ProjectionType::kOrthographic });
data.AddField<light::SceneCamera, float>("orthographic_far_clip_", &light::SceneCamera::orthographic_far_clip_,meta::attribute::EditorCondition{ "projection_type_", (uint64_t)SceneCamera::ProjectionType::kOrthographic });
data.AddField<light::SceneCamera, float>("aspect_ratio_", &light::SceneCamera::aspect_ratio_,meta::attribute::ShowInEditor(false));
data.AddMethod<light::SceneCamera>("SetProjection", &light::SceneCamera::SetProjection);
data.AddMethod<light::SceneCamera>("GetProjection", &light::SceneCamera::GetProjection);
data.AddMethod<light::SceneCamera>("SetViewportSize", &light::SceneCamera::SetViewportSize);
data.AddMethod<light::SceneCamera>("SetPerspective", &light::SceneCamera::SetPerspective);
data.AddMethod<light::SceneCamera>("SetOrthographic", &light::SceneCamera::SetOrthographic);
data.AddMethod<light::SceneCamera>("SetPerspectiveVerticalFov", &light::SceneCamera::SetPerspectiveVerticalFov);
data.AddMethod<light::SceneCamera>("SetPerspectiveNearClip", &light::SceneCamera::SetPerspectiveNearClip);
data.AddMethod<light::SceneCamera>("SetPerspectiveFarClip", &light::SceneCamera::SetPerspectiveFarClip);
data.AddMethod<light::SceneCamera>("SetProjectionType", &light::SceneCamera::SetProjectionType);
data.AddMethod<light::SceneCamera>("GetProjectionType", &light::SceneCamera::GetProjectionType);
data.AddMethod<light::SceneCamera>("GetPerspectiveVerticalFov", &light::SceneCamera::GetPerspectiveVerticalFov);
data.AddMethod<light::SceneCamera>("GetPerspectiveNearClip", &light::SceneCamera::GetPerspectiveNearClip);
data.AddMethod<light::SceneCamera>("GetPerspectiveFarClip", &light::SceneCamera::GetPerspectiveFarClip);
data.AddMethod<light::SceneCamera>("GetOrthographicSize", &light::SceneCamera::GetOrthographicSize);
data.AddMethod<light::SceneCamera>("RecalculateProjection", &light::SceneCamera::RecalculateProjection);
}
};
void Registerscene_camera() {
AutoRegisterTypes();
}
}