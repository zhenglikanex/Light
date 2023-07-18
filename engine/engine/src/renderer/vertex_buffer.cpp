#include "engine/renderer/vertex_buffer.h"

namespace light
{
	VertexBuffer::VertexBuffer(rhi::InputLayout* input_layout, rhi::Buffer* buffer)
		: input_layout_(input_layout)
		, buffer_(buffer)
	{
		LIGHT_ASSERT(buffer_ && buffer_->GetDesc().type == rhi::BufferType::kVertex, "Vertex Buffer Error!");
	}
}