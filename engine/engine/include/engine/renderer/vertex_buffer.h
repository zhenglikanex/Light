#pragma once

#include "engine/rhi/input_layout.h"
#include "engine/rhi/buffer.h"

namespace light
{
	class VertexBuffer : public RefCounter
	{
	public:
		VertexBuffer(rhi::InputLayout* input_layout,rhi::Buffer* buffer);

		rhi::InputLayout* GetInputLayout() const { return input_layout_; }
		rhi::Buffer* GetBuffer() const { return buffer_; }
	private:
		rhi::InputLayoutHandle input_layout_;
		rhi::BufferHandle buffer_;
	};
}