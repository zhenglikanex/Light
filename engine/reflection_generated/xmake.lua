target("ReflectionGenerated")
    set_kind("static")
    add_files("*.cpp")
    add_headerfiles("**.h")
    add_headerfiles("**/**.h")
    add_deps("Engine")
    set_group("engine")
