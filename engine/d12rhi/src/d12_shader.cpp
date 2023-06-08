#include "d12_shader.h"

#include "engine/core/core.h"

#include <cstdio>
#include <d3d12shader.h>
#include <d3d12.h>
#include <d3dcompiler.h>

namespace light::rhi
{
	D12Shader::D12Shader(D12Device* device, const ShaderDesc& desc, std::vector<char> bytecode)
		: Shader(desc, std::move(bytecode))
		, device_(device)
	{
		Handle<ID3D12ShaderReflection> reflection;
		D3DReflect(GetBytecode().data(), GetBytecode().size(), IID_PPV_ARGS(&reflection));

		D3D12_SHADER_DESC shader_desc;
		reflection->GetDesc(&shader_desc);

		for (uint32_t i = 0; i < shader_desc.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC d12_bind_desc;
			reflection->GetResourceBindingDesc(i, &d12_bind_desc);

			ShaderBindResourceDeclaration bind_res_desc;
			bind_res_desc.name = d12_bind_desc.Name;

			switch (d12_bind_desc.Type)
			{
			case D3D_SIT_CBUFFER:
				bind_res_desc.type = ShaderBindResourceType::kConstantBuffer;
				break;
			case D3D_SIT_TBUFFER:
				bind_res_desc.type = ShaderBindResourceType::kTextureBuffer;
				break;
			case D3D_SIT_TEXTURE:
				bind_res_desc.type = ShaderBindResourceType::kTexture;
				break;
			case D3D_SIT_SAMPLER:
				bind_res_desc.type = ShaderBindResourceType::kSampler;
				break;
			default:
				// todo: 支持其他类型
				LIGHT_ASSERT(false, "not support type");
				break;
			}

			bind_res_desc.bind_point = d12_bind_desc.BindPoint;
			bind_res_desc.bind_count = d12_bind_desc.BindCount;
			bind_res_desc.space = d12_bind_desc.Space;

			bind_resources_.emplace_back(bind_res_desc);

			if (bind_res_desc.type == ShaderBindResourceType::kTexture)
			{
				if (bind_res_desc.bind_count == 0) // 说明是动态数组
				{
					// todo:	
				}
				else
				{
					// todo:
				}
			}

			if (bind_res_desc.type == ShaderBindResourceType::kSampler)
			{
				// todo
			}
		}

		for (uint32_t i = 0; i < shader_desc.ConstantBuffers; ++i)
		{
			ID3D12ShaderReflectionConstantBuffer* d12_constant_buffer = reflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC d12_buffer_desc;
			d12_constant_buffer->GetDesc(&d12_buffer_desc);

			if (kMaterialConstantsName == d12_buffer_desc.Name)
			{
				for (uint32_t j = 0; j < d12_buffer_desc.Variables; ++j)
				{
					ID3D12ShaderReflectionVariable* d12_variable = d12_constant_buffer->GetVariableByIndex(j);
					
					D3D12_SHADER_VARIABLE_DESC d12_variable_desc;
					d12_variable->GetDesc(&d12_variable_desc);

					D3D12_SHADER_TYPE_DESC type_desc;
					ID3D12ShaderReflectionType* variable_type = d12_variable->GetType();
					variable_type->GetDesc(&type_desc);

					ShaderParamDeclaration param_desc;
					param_desc.name = d12_variable_desc.Name;
					param_desc.type = type_desc.Name;
					param_desc.offset = d12_variable_desc.StartOffset;
					param_desc.size = d12_variable_desc.Size;
					param_declaractions_.emplace(param_desc.name, param_desc);
				}
			}
		}
	}
}