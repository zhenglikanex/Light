#pragma once

#include "engine/rhi/shader.h"

namespace light::rhi
{
	class D12Device;
	class D12Shader : public Shader
	{
	public:
		D12Shader(D12Device* device, const ShaderDesc& desc, std::vector<char> bytecode);

	private:
		D12Device* device_;
	};
}