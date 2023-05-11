#include "engine/renderer/material.h"

namespace light
{
	MaterialInstance::MaterialInstance(Material* material)
		: material_(material)
	{

	}

	Material::Material(Shader* shader)
		: shader_(shader)
	{
	}
}