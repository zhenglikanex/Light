option("TestOption")
    
option_end()

set_languages("c++20")

add_rules("mode.debug", "mode.release")

package("DirectXTex")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "deps/DirectXTex"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        import("package.tools.cmake").install(package, configs)
    end)
    on_test(function (package)
    end)
package_end()

package("rttr")
    add_deps("cmake")
    set_sourcedir(path.join(os.scriptdir(), "deps/rttr"))
    on_install(function (package)
        local configs = {}
        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
        table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
        table.insert(configs, "-DBUILD_EXAMPLES=OFF")
        import("package.tools.cmake").install(package, configs)
    end)
    on_test(function (package)
    end)
package_end()

add_requires("imgui docking",{debug = true})
add_requires("glm","tinygltf","spdlog","glfw","stb","nlohmann_json","DirectXTex","yaml-cpp")

if is_os("windows") then 
    add_defines("WINDOWS","LIGHT_PLATFORM_WINDOWS")
    add_defines("_CRT_SECURE_NO_WARNINGS")
end

-- 如果当前编译模式是debug
if is_mode("debug") then

    -- 添加DEBUG编译宏
    add_defines("DEBUG")

    -- 启用调试符号
    set_symbols("debug")

    -- 禁用优化
    set_optimize("none")

end

add_defines("UNICODE")

set_rundir(".")


includes("engine")
includes("game")
includes("test")

rule("parser_meta")
    set_kind("project")
    before_build(function (opt)
        -- imports
        import("core.project.config")
        import("core.project.depend")
        import("core.project.project")
        import("core.base.task")
        
        -- run only once for all xmake process in vs
        local tmpfile = path.join(config.buildir(), ".gens", "rules", "plugin.compile_commands.autoupdate")
        local dependfile = tmpfile .. ".d"
        local lockfile = io.openlock(tmpfile .. ".lock")
        if lockfile:trylock() then
            local outputdir
            local sourcefiles = {}
            for _, target in pairs(project.targets()) do
                
                table.join2(sourcefiles, target:sourcefiles(), target:headerfiles())
                local extraconf = target:extraconf("rules", "plugin.compile_commands.autoupdate")
                if extraconf and extraconf.outputdir then
                    outputdir = extraconf.outputdir
                end
            end
            table.sort(sourcefiles)
            depend.on_changed(function ()
                -- we use task instead of os.exec("xmake") to avoid the project lock
                local filename = "compile_commands.json"
                local filepath = outputdir and path.join(outputdir, filename) or filename
                task.run("project", {kind = "compile_commands", outputdir = outputdir})
                print("compile_commands.json updated!")
            end, {dependfile = dependfile,
                  files = project.allfiles(),
                  values = sourcefiles})
            lockfile:close()
        end
        os.exec("tool/metareflect.exe C:/Project/Light/engine/engine/include/engine/scene/components.h C:/Project/Light/engine/engine/include/engine/scene/scene_camera.h -p C:/Project/Light/compile_commands.json")
    end)
rule_end()

add_rules("parser_meta",{outputdir="."})
--
-- If you want to known more usage about xmake, please see https://xmake.io
--
-- ## FAQ
--
-- You can enter the project directory firstly before building project.
--
--   $ cd projectdir
--
-- 1. How to build project?
--
--   $ xmake
--
-- 2. How to configure project?
--
--   $ xmake f -p [macosx|linux|iphoneos ..] -a [x86_64|i386|arm64 ..] -m [debug|release]
--
-- 3. Where is the build output directory?
--
--   The default output directory is `./build` and you can configure the output directory.
--
--   $ xmake f -o outputdir
--   $ xmake
--
-- 4. How to run and debug target after building project?
--
--   $ xmake run [targetname]
--   $ xmake run -d [targetname]
--
-- 5. How to install target to the system directory or other output directory?
--
--   $ xmake install
--   $ xmake install -o installdir
--
-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro defination
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--

