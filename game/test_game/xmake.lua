target("TestDep")
    set_kind("static")
    add_files("test1.cpp")

target("TestGame")
    set_kind("binary")
    add_deps("GlfwAppaction","TestDep")
    add_files("test2.cpp")