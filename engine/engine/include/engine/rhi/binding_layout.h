#pragma once

#include <vector>

#include "types.h"
#include "resource.h"

namespace light::rhi
{
	struct BindingParameter
	{
		struct DescriptorRange
		{
			DescriptorRangeType range_type = DescriptorRangeType::kConstantsBufferView;
			uint32_t num_descriptors = 1;
			uint32_t base_shader_register = 0;
			uint32_t register_space = 0;
			bool is_volatile = false;	// 是否动态可变(比如设置的描述符数小于num_descriptors)
		};
		
		struct DescriptorTable
		{
			uint32_t num_descriptor_ranges;
			const DescriptorRange* descriptor_ranges;
		};

		struct Constants
		{
			uint32_t shader_register;
			uint32_t register_space;
			uint32_t num32_bit_values;
		};

		struct Descriptor
		{
			uint32_t shader_register;
			uint32_t register_space;
		};

		BindingParameterType type;
		ShaderVisibility shader_visibility;
		union
		{
			DescriptorTable descriptor_table;
			Constants constants;
			Descriptor descriptor;
		};

		void InitAsDescriptorTable(uint32_t num_descriptor_ranges,
			const DescriptorRange* descriptor_ranges,
			ShaderVisibility visibility = ShaderVisibility::kAll)
		{
			type = BindingParameterType::kDescriptorTable;
			shader_visibility = visibility;
			descriptor_table.num_descriptor_ranges = num_descriptor_ranges;
			descriptor_table.descriptor_ranges = descriptor_ranges;
		}

		void InitAsConstants(
			uint32_t num32_bit_values,
			uint32_t shader_register,
			uint32_t register_space = 0,
			ShaderVisibility visibility = ShaderVisibility::kAll) noexcept
		{
			type = BindingParameterType::kConstants;
			shader_visibility = visibility;
			constants.num32_bit_values = num32_bit_values;
			constants.shader_register = shader_register;
			constants.register_space = register_space;
		}

		void InitAsConstantBufferView(
			uint32_t shader_register,
			uint32_t register_space = 0,
			// todo: D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
			ShaderVisibility visibility = ShaderVisibility::kAll) noexcept
		{
			type = BindingParameterType::kConstantBufferView;
			shader_visibility = visibility;
			descriptor.shader_register = shader_register;
			descriptor.register_space = register_space;
		}

		void InitAsShaderResourceView(
			uint32_t shader_register,
			uint32_t register_space = 0,
			// todo: D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
			ShaderVisibility visibility = ShaderVisibility::kAll) noexcept
		{
			type = BindingParameterType::kShaderResourceView;
			shader_visibility = visibility;
			descriptor.shader_register = shader_register;
			descriptor.register_space = register_space;
		}

		void InitAsUnorderedAccessView(
			uint32_t shader_register,
			uint32_t register_space = 0,
			// todo :D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE,
			ShaderVisibility visibility = ShaderVisibility::kAll) noexcept
		{
			type = BindingParameterType::kUnorderAccessView;
			shader_visibility = visibility;
			descriptor.shader_register = shader_register;
			descriptor.register_space = register_space;
		}
	};

	class BindingLayout final : public Resource
	{
	public:
		explicit BindingLayout(uint32_t num_parameters)
		{
			parameters_.resize(num_parameters);
		}

		~BindingLayout() override = default;

		BindingLayout(uint32_t num_parameters,const BindingParameter* paramater)
		{
			parameters_.reserve(num_parameters);

			for (uint32_t i = 0; i < num_parameters; ++i)
			{
				parameters_.emplace_back(*(paramater + i));
			}
		}

		void Add(uint32_t parameter_index, const BindingParameter& parameter)
		{
			if (parameters_.size() <= parameter_index)
			{
				parameters_.resize(parameter_index + 1);
			}

			parameters_[parameter_index] = parameter;
		}

		auto operator[](size_t index) const { return parameters_[index]; }

		size_t Size() const { return parameters_.size(); }

		auto begin() const { return parameters_.begin(); }

		auto end() const { return parameters_.end(); }
	private:
		std::vector<BindingParameter> parameters_;
	};

	using BindingLayoutHandle = Handle<BindingLayout>;
}