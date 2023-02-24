target("Model")
    set_kind("binary")
    add_files("*.cpp")
    add_files("../rhi_base.cpp")
    add_headerfiles("../rhi_base.h")
    add_headerfiles("../model.h")
    add_deps("D12RHI")
    add_packages("glfw","glm","tinygltf","spdlog")
    set_group("test")

    