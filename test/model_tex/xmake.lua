target("ModelTex")
    set_kind("binary")
    add_files("*.cpp")
    add_files("../rhi_base.cpp")
    add_headerfiles("../rhi_base.h")
    add_headerfiles("../mesh.h")
    add_headerfiles("../texture_loader.h")
    add_deps("D12RHI")
    add_packages("glfw","glm","tinygltf","spdlog","DirectXTex","stb")
    set_group("test")

    