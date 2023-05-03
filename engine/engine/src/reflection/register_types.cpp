#include "glm/glm.hpp"

#include "engine/reflection/registry.h"
using namespace light;
using namespace meta;
namespace meta_generated {
	void AutoAddTypes() {
	}
	struct ReflectionBuilder {
		ReflectionBuilder() {
			{
				TypeData& data = Registry::Get().AddTypeData<glm::vec4>("glm::vec4");
				data.AddField<glm::vec4, float>("x", &glm::vec4::x);
				data.AddField<glm::vec4, float>("y", &glm::vec4::y);
				data.AddField<glm::vec4, float>("z", &glm::vec4::z);
				data.AddField<glm::vec4, float>("w", &glm::vec4::w);
			}

			{
				TypeData& data = Registry::Get().AddTypeData<glm::vec3>("glm::vec3");
				data.AddField<glm::vec3, float>("x", &glm::vec3::x);
				data.AddField<glm::vec3, float>("y", &glm::vec3::y);
				data.AddField<glm::vec3, float>("z", &glm::vec3::z);
			}

			{
				TypeData& data = Registry::Get().AddTypeData<glm::vec2>("glm::vec2");
				data.AddField<glm::vec2, float>("x", &glm::vec2::x);
				data.AddField<glm::vec2, float>("y", &glm::vec2::y);
			}

			{
				TypeData& data = Registry::Get().AddTypeData<glm::mat4>("glm::mat4");
			}

			{
				TypeData& data = Registry::Get().AddTypeData<glm::mat3>("glm::mat3");
			}
		}
	};
	ReflectionBuilder builder;
}