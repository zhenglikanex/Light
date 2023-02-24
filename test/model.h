#pragma once

#include "tiny_gltf.h"
#include "glm/glm.hpp"
#include "spdlog/spdlog.h"

#include "rhi/input_layout.h"
#include "rhi/buffer.h"

using namespace light::rhi;

struct Mesh
{
	std::vector<light::rhi::BufferHandle> vertex_buffers;
	light::rhi::BufferHandle index_buffer;
};

class GLTFReader
{
public:
	
	std::vector<Mesh> Load(std::string_view filename);
	

private:
	tinygltf::Model* model_;
};

inline std::vector<Mesh> LoadScene(std::string_view filename)
{
	tinygltf::Model model;
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename.data());
	if (!warn.empty()) {
		spdlog::warn(err);
	}

	if (!err.empty()) {
		spdlog::error(err);
	}

	if (!res)
	{
		return {};
	}

	std::vector<Mesh> meshs;

	const tinygltf::Scene& scene = model.scenes[model.defaultScene];
	for (auto node_index : scene.nodes)
	{
		auto& node = model.nodes[node_index];
		LoadNode(meshs,node);
	}
	return meshs;
}

inline void LoadMesh()
{
	
}

inline void LoadNode(std::vector<Mesh>& meshs,const Model const tinygltf::Node& node)
{
	
}
