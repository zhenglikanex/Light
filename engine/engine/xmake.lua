target("Engine")
    set_kind("static")
    add_deps("Log")
    if is_os("windows") then 
        add_deps("WindowsWindow")
    end
    add_packages("glm")
    add_files("*/*.cpp")
    add_headerfiles("**/**.h")
    add_includedirs("include",{public = true})
    set_group("engine")