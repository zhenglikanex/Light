#pragma once

#include "light_pch.h"

#include "engine/core/core.h"
#include "engine/renderer/material.h"
#include "engine/rhi/buffer.h"

#include "glm/glm.hpp"

namespace light
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 texcoord;
	};

	class Mesh : public RefCounter
	{
	public:
		Mesh(std::string_view filename);

		void SetMaterial(Material* material) { material_ = material; }

		rhi::Buffer* GetVertexBuffer() const { return vertex_buffer_; }
		rhi::Buffer* GetIndexBuffer() const { return index_buffer_; }
		Material* GetMaterial() const { return material_; }
	private:
		std::vector<Vertex> vertices_;
		std::vector<uint32_t> indices_;
		rhi::BufferHandle vertex_buffer_;
		rhi::BufferHandle index_buffer_;
		Ref<Material> material_;
	};
}