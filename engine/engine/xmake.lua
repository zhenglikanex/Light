target("Engine")
    set_kind("static")
    add_includedirs("include/",{ public = true })
    add_includedirs("../../3rdparty",{ public = true })
    add_packages("glm","imgui","spdlog","stb","DirectXTex",{public = true})
    set_pcxxheader("include/light_pch.h")
    add_files("*/*.cpp")
    add_files("**/**.cpp")
    add_headerfiles("**/**.h")
    set_group("engine")