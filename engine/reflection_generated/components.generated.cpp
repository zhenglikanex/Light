/* this file is auto-generated. do not edit! */
#include "engine/reflection/registry.h"
#include "reflection_generated.h"
#include"C:\Project\Light\engine\engine\include\engine\scene\components.h"
using namespace light;
using namespace meta;
namespace meta_generated { 
static void AutoRegisterTypes() { 
{
TypeData &data = Registry::Get().AddTypeData<light::Component>("light::Component");
}
{
TypeData &data = Registry::Get().AddTypeData<light::SpriteRendererComponent>("light::SpriteRendererComponent");
data.AddBaseType("light::Component");
data.AddField<light::SpriteRendererComponent, glm::vec<4, float>>("color", &light::SpriteRendererComponent::color);
}
{
TypeData &data = Registry::Get().AddTypeData<light::TransformComponent>("light::TransformComponent");
data.AddBaseType("light::Component");
data.AddField<light::TransformComponent, glm::vec<3, float>>("position", &light::TransformComponent::position);
data.AddField<light::TransformComponent, glm::vec<3, float>>("rotation", &light::TransformComponent::rotation);
data.AddField<light::TransformComponent, glm::vec<3, float>>("scale", &light::TransformComponent::scale);
data.AddMethod<light::TransformComponent>("GetTransform", &light::TransformComponent::GetTransform);
}
{
TypeData &data = Registry::Get().AddTypeData<light::TagComponent>("light::TagComponent");
data.AddBaseType("light::Component");
data.AddField<light::TagComponent, std::basic_string<char>>("tag", &light::TagComponent::tag);
}
{
TypeData &data = Registry::Get().AddTypeData<light::CameraComponent>("light::CameraComponent");
data.AddBaseType("light::Component");
data.AddField<light::CameraComponent, light::SceneCamera>("camera", &light::CameraComponent::camera);
data.AddField<light::CameraComponent, bool>("primary", &light::CameraComponent::primary);
data.AddField<light::CameraComponent, bool>("aspect_ratio_fixed", &light::CameraComponent::aspect_ratio_fixed);
}
{
TypeData &data = Registry::Get().AddTypeData<light::LightComponent>("light::LightComponent");
data.AddBaseType("light::Component");
data.AddField<light::LightComponent, glm::vec<3, float>>("color", &light::LightComponent::color);
}
};
void Registercomponents() {
AutoRegisterTypes();
}
}