#include "engine/renderer/mesh.h"

#include "engine/core/application.h"
#include "engine/renderer/renderer.h"

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

	Mesh::Mesh(std::string_view filename)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filename.data(), ImportFlags);
		if (!scene || !scene->HasMeshes())
		{
			LOG_ENGINE_ERROR("Failed to load mesh file:{0}", filename);
			return;
		}

		aiMesh* mesh = scene->mMeshes[0];
		
		vertices_.reserve(mesh->mNumVertices);

		for (int i = 0; i < vertices_.capacity(); ++i)
		{
			Vertex vertex;
			vertex.position = { mesh->mVertices[i].x,mesh->mVertices[i].y,mesh->mVertices[i].z };
			vertex.normal = { mesh->mNormals[i].x,mesh->mNormals[i].y,mesh->mNormals[i].z };
			vertex.texcoord = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
			vertex.bitangent = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };

			vertices_.emplace_back(vertex);
		}

		indices_.reserve(mesh->mNumFaces * 3);

		//现在通过每个网格的面（一个面是一个网格它的三角形）并检索相应的顶点索引
		for (uint32_t i = 0; i < mesh->mNumFaces; ++i) 
		{
			aiFace face = mesh->mFaces[i];
			for (uint32_t j = 0; j < face.mNumIndices; j++)
			{
				indices_.emplace_back(face.mIndices[j]);
			}
		}

		rhi::Device* device = Application::Get().GetDevice();
		
		rhi::BufferDesc vertex_buffer_desc;
		vertex_buffer_desc.type = rhi::BufferType::kVertex;
		vertex_buffer_desc.size_in_bytes = vertices_.size() * sizeof(Vertex);
		vertex_buffer_desc.stride = sizeof(Vertex);

		vertex_buffer_ = device->CreateBuffer(vertex_buffer_desc);

		rhi::BufferDesc index_buffer_desc;
		index_buffer_desc.type = rhi::BufferType::kIndex;
		index_buffer_desc.size_in_bytes = indices_.size() * sizeof(uint32_t);
		index_buffer_desc.stride = sizeof(uint32_t);

		index_buffer_ = device->CreateBuffer(index_buffer_desc);

		auto command_list = device->GetCommandList(rhi::CommandListType::kCopy);
		command_list->WriteBuffer(vertex_buffer_, (uint8_t*)vertices_.data(), vertices_.size() * sizeof(Vertex));
		command_list->WriteBuffer(index_buffer_, (uint8_t*)indices_.data(), indices_.size() * sizeof(uint32_t));
		command_list->ExecuteCommandList();
	}
}