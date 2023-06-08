#pragma once

#include "light_pch.h"
#include "engine/scene/scene.h"

#include "yaml-cpp/yaml.h"

namespace light
{
	class SceneSerializer
	{
	public:
		SceneSerializer(Scene* scene);

		void SerializeText(const std::string& filepath);
		bool DeserializeText(const std::string& filepath);
	private:
		Ref<Scene> scene_;
	};
}