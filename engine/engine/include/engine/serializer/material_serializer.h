#pragma once

#include "light_pch.h"
#include "engine/renderer/material.h"

#include "yaml-cpp/yaml.h"

namespace light
{
	class MaterialSerializer
	{
	public:
		MaterialSerializer(Material* material);

		void SerializeText(const std::string& filepath);
		bool DeserializeText(const std::string& filepath);
	private:
		Ref<Material> material_;
	};
}