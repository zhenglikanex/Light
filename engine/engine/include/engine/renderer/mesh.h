#pragma once

#include "light_pch.h"

#include "engine/core/base.h"
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

	struct SubMesh
	{
		uint32_t base_vertex;
		uint32_t base_index;
		uint32_t index_count;
		uint32_t material_index;
	};

	class Mesh : public RefCounter
	{
	public:
		Mesh(std::string_view filename);

		void SetMaterial(uint32_t index, Material* material);

		const std::string& GetFileName() const { return filename_; }

		rhi::Buffer* GetVertexBuffer() const { return vertex_buffer_; }
		rhi::Buffer* GetIndexBuffer() const { return index_buffer_; }
		Material* GetMaterial(uint32_t index) const { return materials_[index]; }
		const SubMesh& GetSubMesh(uint32_t index) const { return sub_meshes_[index]; }
		uint32_t GetNumSubMesh() const { return sub_meshes_.size(); }

		auto begin() const { return sub_meshes_.begin(); }
		auto end() const { return sub_meshes_.end(); }
	private:
		std::string filename_;
		std::vector<Vertex> vertices_;
		std::vector<uint32_t> indices_;
		rhi::BufferHandle vertex_buffer_;
		rhi::BufferHandle index_buffer_;
		std::vector<Ref<Material>> materials_;
		std::vector<SubMesh> sub_meshes_;
	};
}