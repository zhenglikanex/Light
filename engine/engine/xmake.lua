target("Engine")
    set_kind("static")
    add_includedirs("include/",{ public = true })
    add_packages("glm","imgui","spdlog",{public = true})
    add_files("*/*.cpp")
    add_headerfiles("**/**.h")
    set_group("engine")