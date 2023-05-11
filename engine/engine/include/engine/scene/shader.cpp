#include "engine/renderer/shader.h"

namespace light
{
	Shader::Shader(rhi::Shader* vs, rhi::Shader* ps, rhi::Shader* gs)
		: vs_(vs)
		, ps_(ps)
		, gs_(gs)
	{
		
	}
}