#pragma once

#include "types.h"
#include "engine/core/base.h"
#include "render_target.h"
#include "shader.h"
#include "input_layout.h"
#include "binding_layout.h"

namespace light::rhi
{
	struct GraphicsPipelineDesc
	{
		PrimitiveTopology primitive_type = PrimitiveTopology::kTriangleList;
		InputLayoutHandle input_layout;
		BindingLayoutHandle binding_layout;

		ShaderHandle vs;
		ShaderHandle ps;
		ShaderHandle ds;
		ShaderHandle hs;
		ShaderHandle gs;

		RasterizerDesc rasterizer_state;
		BlendDesc blend_state;
		DepthStencilDesc depth_stencil_state;
	};

	class GraphicsPipeline : public RefCounter
	{
	public:
		GraphicsPipeline(const GraphicsPipelineDesc& desc)
			: desc_(desc)
		{
			
		}

		const GraphicsPipelineDesc& GetDesc() const { return desc_; }
	protected:
		GraphicsPipelineDesc desc_;
	};

	using GraphicsPipelineHandle = Ref<GraphicsPipeline>;
	
}