#pragma once

#include "light_pch.h"

#include "engine/core/base.h"
#include "engine/renderer/material.h"
#include "engine/rhi/buffer.h"
#include "engine/renderer/vertex_buffer.h"

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

	struct SkinnedVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 tangent;
		glm::vec3 bitangent;
		glm::vec2 texcoord;
		glm::ivec4 bones;
		glm::vec4 weights;
	};

	struct SubMesh
	{
		uint32_t base_vertex;
		uint32_t base_index;
		uint32_t index_count;
		uint32_t material_index;
	};

	struct Bone
	{
		std::string name;
		glm::mat4 transform;
		int32_t parent;		// -1表示没有parent

		Bone(std::string_view name, const glm::mat4& transform, int32_t parent)
			: name(name.data())
			, transform(transform)
			, parent(parent)
		{

		}
	};

	class Skeleton
	{
	public:
		Skeleton(std::vector<Bone>&& bones);

		void ComputeGlobalBoneBindPose();
	private:
		std::vector<Bone> bones_;
		std::vector<glm::mat4> global_bone_bind_poses_;
	};

	class Mesh : public Asset
	{
	public:
		static AssetType StaticType()
		{
			return AssetType::kMesh;
		}

		static const char* StaticName()
		{
			return "Mesh";
		}

		Mesh(std::string_view filename);

		AssetType GetAssetType() const override { return AssetType::kMesh; }

		void SetMaterial(uint32_t index, Material* material);

		VertexBuffer* GetVertexBuffer() const { return vertex_buffer_; }

		rhi::Buffer* GetIndexBuffer() const { return index_buffer_; }

		Material* GetMaterial(uint32_t index) const { return materials_[index]; }

		const SubMesh& GetSubMesh(uint32_t index) const { return sub_meshes_[index]; }

		uint32_t GetNumSubMesh() const { return sub_meshes_.size(); }

		bool HasSkeleton() const { return skeleton_ != nullptr; }

		auto begin() const { return sub_meshes_.begin(); }

		auto end() const { return sub_meshes_.end(); }
	private:
		std::unique_ptr<Skeleton> skeleton_;
		std::vector<Vertex> vertices_;
		std::vector<SkinnedVertex> skinned_vertices_;
		std::vector<uint32_t> indices_;
		Ref<VertexBuffer> vertex_buffer_;
		rhi::BufferHandle index_buffer_;
		std::vector<Ref<Material>> materials_;
		std::vector<SubMesh> sub_meshes_;
	};
}