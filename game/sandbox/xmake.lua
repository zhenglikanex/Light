target("Sandbox")
    set_kind("binary")
    add_files("**/**.cpp")
    add_deps("Engine")
    if is_os("windows") then
        add_deps("Platform")
    end
    add_deps("D12RHI")
    set_group("game")
    add_ldflags("/subsystem:console")
    add_cxflags("/utf-8")