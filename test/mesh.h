#pragma once

#undef max
#undef min

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "spdlog/spdlog.h"

#include "rhi/device.h"
#include "rhi/buffer.h"

using namespace light::rhi;

enum class VertexSlot
{
	kPosition, 
};

struct Mesh
{
	Mesh() = default;
	~Mesh() = default;

	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	Mesh(Mesh&&) = default;
	Mesh& operator=(Mesh&&) = default;

	[[nodiscard]] uint32_t GetIndexCount() const
	{
		if(index_buffer)
		{
			auto& desc = index_buffer->GetDesc();
			uint32_t bytes = desc.format == Format::R16_UINT ? 2 : 4;
			return desc.size_in_bytes / bytes;
		}

		return 0;
	}

	glm::mat4 model_matrix = glm::mat4(
		1.0f, 0.f, 0.0f, 0.0f,
		0.0f, 1.f, 0.0f, 0.0f,
		0.0f, 0.f, -1.0f, 0.0f,
		0.0f, 0.f, 0.0f, 1.0f);
	PrimitiveTopology primitive = PrimitiveTopology::kTriangleList;
	std::vector<BufferHandle> vertex_buffers;
	BufferHandle index_buffer;
};

class GLTFReader
{
public:
	explicit GLTFReader(Device* device)
		: device_(device)
		, command_(nullptr)
		, model_(nullptr)
	{
		
	}

	~GLTFReader()
	{
		delete model_;
	}

	bool Load(std::string_view filename)
	{
		model_ = new tinygltf::Model();
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		bool res = loader.LoadASCIIFromFile(&*model_, &err, &warn, filename.data());
		if (!warn.empty()) {
			//std::cout << "WARN: " << warn << std::endl;
		}

		if (!err.empty()) {
			//std::cout << "ERR: " << err << std::endl;
		}

		return res;
	}

	[[nodiscard]] std::vector<Mesh> GetMeshes()
	{
		std::vector<Mesh> meshes;

		command_ = device_->GetCommandList(CommandListType::kDirect);

		const tinygltf::Scene& scene = model_->scenes[model_->defaultScene];
		for (auto node_index : scene.nodes)
		{
			auto& node = model_->nodes[node_index];
			LoadNode(meshes, node);
		}

		command_->ExecuteCommandList();

		return meshes;
	}
private:

	void LoadNode(std::vector<Mesh>& meshes,const tinygltf::Node& node) const
	{
		if ((node.mesh >= 0) && (node.mesh < model_->meshes.size())) {
			LoadMesh(meshes, model_->meshes[node.mesh]);
		}

		for(auto index : node.children)
		{
			LoadNode(meshes, model_->nodes[index]);
		}
	}

	void LoadMesh(std::vector<Mesh>& meshes,const tinygltf::Mesh& gltf_mesh) const
	{
		Mesh& mesh = meshes.emplace_back();
		for(auto& primitive : gltf_mesh.primitives)
		{
			switch (primitive.mode)
			{
			case TINYGLTF_MODE_POINTS:
				mesh.primitive = PrimitiveTopology::kPointList;
				break;
			case TINYGLTF_MODE_LINE:
				mesh.primitive = PrimitiveTopology::kLineList;
				break;
			case TINYGLTF_MODE_TRIANGLES:
				mesh.primitive = PrimitiveTopology::kTriangleList;
				break;
			case TINYGLTF_MODE_TRIANGLE_STRIP:
				mesh.primitive = PrimitiveTopology::kTriangleStrip;
				break;
			case TINYGLTF_MODE_TRIANGLE_FAN:
				mesh.primitive = PrimitiveTopology::kTriangleFan;
				break;
			default:
				CHECK(false, "not support primitive");
			}

			// build vertex buffers
			for (auto& [name,access_index] : primitive.attributes)
			{
				auto& accessor = model_->accessors[access_index];
				auto& buffer_view = model_->bufferViews[accessor.bufferView];
				auto& buffer = model_->buffers[buffer_view.buffer];

				int32_t slot = -1;
				if (name == "POSITION") slot = 0;
				if (name == "NORMAL") slot = 1;
				if (name == "TEXCOORD_0") slot = 2;

				if(slot >= 0)
				{
					if(slot >= mesh.vertex_buffers.size())
					{
						mesh.vertex_buffers.resize(slot + 1);
					}

					if(accessor.count)
					{
						
					}
					
					BufferDesc desc;
					desc.type = BufferType::kVertex;
					

#define CHOOSE_FORMAT(FORMAT,STRIDE,TYPE,COMPONENT,NORMALIZED)\
	if(accessor.type == TYPE && accessor.componentType == COMPONENT && accessor.normalized == NORMALIZED) { desc.format = FORMAT; desc.stride = STRIDE; }
					
					CHOOSE_FORMAT(Format::RG8_UINT,2, TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, false)
					CHOOSE_FORMAT(Format::RG8_UNORM, 2, TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, true)
					CHOOSE_FORMAT(Format::RG8_SINT, 2,TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_BYTE, false)
					CHOOSE_FORMAT(Format::RG8_SNORM, 2,TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_BYTE, true)
					CHOOSE_FORMAT(Format::RGBA8_UINT, 4,TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, false)
					CHOOSE_FORMAT(Format::RGBA8_UNORM, 4,TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE, true)
					CHOOSE_FORMAT(Format::RGBA8_SINT, 4,TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_BYTE, false)
					CHOOSE_FORMAT(Format::RGBA8_SNORM, 4,TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_BYTE, true)
					CHOOSE_FORMAT(Format::RG16_UINT, 4,TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, false)
					CHOOSE_FORMAT(Format::RG16_UNORM, 4,TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, true)
					CHOOSE_FORMAT(Format::RG16_SINT, 4,TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_SHORT, false)
					CHOOSE_FORMAT(Format::RG16_SNORM, 4,TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_SHORT, true)
					CHOOSE_FORMAT(Format::RGBA16_UINT, 8,TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, false)
					CHOOSE_FORMAT(Format::RGBA16_UNORM,8, TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT, true)
					CHOOSE_FORMAT(Format::RGBA16_SINT, 8,TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_SHORT, false)
					CHOOSE_FORMAT(Format::RGBA16_SNORM,8, TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_SHORT, true)
					CHOOSE_FORMAT(Format::RG32_UINT, 8,TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, false)
					CHOOSE_FORMAT(Format::RG32_SINT, 8,TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_INT, false)
					CHOOSE_FORMAT(Format::RGB32_UINT, 12,TINYGLTF_TYPE_VEC3, TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, false)
					CHOOSE_FORMAT(Format::RGB32_SINT, 12,TINYGLTF_TYPE_VEC3, TINYGLTF_COMPONENT_TYPE_INT, false)
					CHOOSE_FORMAT(Format::RGBA32_UINT, 16,TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT, false)
					CHOOSE_FORMAT(Format::RGBA32_SINT, 16,TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_INT, false)
					CHOOSE_FORMAT(Format::RG32_FLOAT,8, TINYGLTF_TYPE_VEC2, TINYGLTF_COMPONENT_TYPE_FLOAT, false)
					CHOOSE_FORMAT(Format::RGB32_FLOAT,12, TINYGLTF_TYPE_VEC3, TINYGLTF_COMPONENT_TYPE_FLOAT, false)
					CHOOSE_FORMAT(Format::RGBA32_FLOAT,16, TINYGLTF_TYPE_VEC4, TINYGLTF_COMPONENT_TYPE_FLOAT, false)
#undef CHOOSE_FORMAT
					desc.size_in_bytes = static_cast<uint32_t>(buffer_view.byteLength);

					mesh.vertex_buffers[slot] = device_->CreateBuffer(desc);
					command_->WriteBuffer(mesh.vertex_buffers[slot], buffer.data.data() + buffer_view.byteOffset, buffer_view.byteLength);
				}
			}

			// build index buffer
			auto& accessor = model_->accessors[primitive.indices];
			auto& buffer_view = model_->bufferViews[accessor.bufferView];
			auto& buffer = model_->buffers[buffer_view.buffer];

			BufferDesc desc;
			desc.type = BufferType::kIndex;
			

			if(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
			{
				desc.format = Format::R16_UINT;
				desc.stride = 2;
			}
			if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
			{
				desc.format = Format::R32_UINT;
				desc.stride = 4;
			}

			desc.size_in_bytes = static_cast<uint32_t>(buffer_view.byteLength);

			mesh.index_buffer = device_->CreateBuffer(desc);
			command_->WriteBuffer(mesh.index_buffer, buffer.data.data() + buffer_view.byteOffset, buffer_view.byteLength);
		}
	}

	DeviceHandle device_;
	CommandListHandle command_;
	tinygltf::Model* model_ = nullptr;
};
