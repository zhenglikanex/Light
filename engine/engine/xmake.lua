target("Engine")
    set_kind("static")
    if is_os("windows") then 
        add_deps("WindowsWindow")
        add_defines("WINDOWS")
    end
    add_packages("glm","imgui")
    add_files("*/*.cpp")
    add_headerfiles("**/**.h")
    add_includedirs("include",{public = true})
    set_group("engine")