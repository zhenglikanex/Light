#include "engine/renderer/mesh.h"

#include "engine/core/application.h"
#include "engine/renderer/renderer.h"
#include "engine/asset/asset_manager.h"

#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"

namespace light
{
	namespace {
		const unsigned int ImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_Debone |
			aiProcess_ValidateDataStructure;
	}

	static rhi::InputLayoutHandle s_mesh_input_layout;

	Mesh::Mesh(std::string_view filename)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename.data(), ImportFlags);
		if (!scene || !scene->HasMeshes())
		{
			LOG_ENGINE_ERROR("Failed to load mesh file:{0}", filename);
			return;
		}

		materials_.resize(scene->mNumMeshes);
		sub_meshes_.resize(scene->mNumMeshes);
		
		uint32_t num_vertices = 0;
		uint32_t num_indices = 0;
		for (int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index)
		{
			aiMesh* mesh = scene->mMeshes[mesh_index];
			num_vertices += mesh->mNumVertices;
			num_indices += mesh->mNumFaces * 3;
		}

		vertices_.reserve(num_vertices);
		indices_.reserve(num_indices);

		for (int mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index)
		{
			aiMesh* mesh = scene->mMeshes[mesh_index];

			sub_meshes_[mesh_index].base_vertex = vertices_.size();
			sub_meshes_[mesh_index].base_index = indices_.size();
			sub_meshes_[mesh_index].index_count = mesh->mNumFaces * 3;
			sub_meshes_[mesh_index].material_index = mesh_index;

			LIGHT_ASSERT(mesh->HasPositions() && mesh->HasNormals(), "必须含有Position和Normals!");

			for (int i = 0; i < mesh->mNumVertices; ++i)
			{
				Vertex vertex;
				vertex.position = { mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z };
				vertex.normal = { mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z };

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					vertex.bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
				}
				
				if (mesh->HasTextureCoords(0))
				{
					vertex.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
				}

				vertices_.emplace_back(vertex);
			}

			//现在通过每个网格的面（一个面是一个网格它的三角形）并检索相应的顶点索引
			for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
			{
				aiFace face = mesh->mFaces[i];
				for (uint32_t j = 0; j < face.mNumIndices; j++)
				{
					indices_.emplace_back(face.mIndices[j]);
				}
			}

			// todo 改用内置shader
			Ref<Material> material = MakeRef<Material>(AssetManager::LoadAsset<Shader>("shaders/simplepbr.shader"));
			if (scene->HasMaterials())
			{
				std::filesystem::path path = filename;
				path = path.parent_path();
				path = AssetManager::GetAssetRelativePath(path);

				aiMaterial* ai_material = scene->mMaterials[mesh->mMaterialIndex];
				
				aiColor3D base_color;
				if (ai_material->Get(AI_MATKEY_BASE_COLOR, base_color) == aiReturn_SUCCESS)
				{
					material->Set("cbAlbedoColor", glm::vec3(base_color.r, base_color.g, base_color.b));
				}

				aiString base_color_tex;
				if (ai_material->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &base_color_tex) == aiReturn_SUCCESS)
				{
					if (auto texture = AssetManager::LoadAsset<rhi::Texture>((path / base_color_tex.C_Str()).generic_string()))
					{
						material->Set("gAbledoMap", texture);
					}
				}

				ai_real metallic = 0;
				if (ai_material->Get(AI_MATKEY_METALLIC_FACTOR, metallic) == aiReturn_SUCCESS)
				{
					material->Set("cbMetalness", metallic);
				}

				aiString metallic_tex;
				if (ai_material->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &metallic_tex) == aiReturn_SUCCESS)
				{
					if (auto texture = AssetManager::LoadAsset<rhi::Texture>((path / metallic_tex.C_Str()).generic_string()))
					{
						material->Set("gMetalnessMap", texture);
					}
				}

				ai_real roughness;
				if (ai_material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness) == aiReturn_SUCCESS)
				{
					material->Set("cbRoughness", roughness);
				}

				aiString roughness_tex;
				if (ai_material->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &roughness_tex) == aiReturn_SUCCESS)
				{
					if (auto texture = AssetManager::LoadAsset<rhi::Texture>((path / roughness_tex.C_Str()).generic_string()))
					{
						material->Set("gRoughnessMap", texture);
					}
				}
			}

			materials_[mesh_index] = material;
		}
		
		rhi::Device* device = Application::Get().GetDevice();

		rhi::BufferDesc vertex_buffer_desc;
		vertex_buffer_desc.type = rhi::BufferType::kVertex;
		vertex_buffer_desc.size_in_bytes = vertices_.size() * sizeof(Vertex);
		vertex_buffer_desc.stride = sizeof(Vertex);

		rhi::BufferHandle vertex_buffer = device->CreateBuffer(vertex_buffer_desc);

		rhi::BufferDesc index_buffer_desc;
		index_buffer_desc.format = rhi::Format::R32_UINT;
		index_buffer_desc.type = rhi::BufferType::kIndex;
		index_buffer_desc.size_in_bytes = indices_.size() * sizeof(uint32_t);
		index_buffer_desc.stride = sizeof(uint32_t);

		index_buffer_ = device->CreateBuffer(index_buffer_desc);

		auto command_list = device->GetCommandList(rhi::CommandListType::kCopy);
		command_list->WriteBuffer(vertex_buffer, (uint8_t*)vertices_.data(), vertices_.size() * sizeof(Vertex));
		command_list->WriteBuffer(index_buffer_, (uint8_t*)indices_.data(), indices_.size() * sizeof(uint32_t));
		command_list->ExecuteCommandList();

		if (!s_mesh_input_layout)
		{
			std::vector<rhi::VertexAttributeDesc> vertex_attributes;
			vertex_attributes.reserve(5);

			vertex_attributes = {
				{ "POSITION",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,position),false},
				{ "NORMAL",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,normal),false },
				{ "TANGENT",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,tangent),false },
				{ "BINORMAL",0,rhi::Format::RGB32_FLOAT,0,offsetof(Vertex,bitangent),false },
				{ "TEXCOORD",0,rhi::Format::RG32_FLOAT,0,offsetof(Vertex,texcoord),false},
			};

			s_mesh_input_layout = Application::Get().GetDevice()->CreateInputLayout(std::move(vertex_attributes));
		}

		vertex_buffer_ = MakeRef<VertexBuffer>(s_mesh_input_layout, vertex_buffer);
	}

	void Mesh::SetMaterial(uint32_t index, Material* material)
	{
		LIGHT_ASSERT(index < materials_.size(), "out range!");

		materials_[index] = material;
	}
}