include "../../../../Premake/extensions.lua"
-- include for common stuff 
include "../../../../Premake/common.lua"
-------------------------------------------------------------
project "GroundBreakerScriptEditor"
location (dirs.projectfiles)
dependson { "FeatherEngine, MuninGraph,MuninGraphEditor, ScriptGraphNodes, ImGuiNodeEditor"}

kind "ConsoleApp"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.bin}")
targetdir ("%{dirs.bin}")
targetname("%{prj.name}")
objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")

libdirs { dirs.lib, dirs.utilities.."TGAFbx/lib/%{cfg.buildcfg}", dirs.lib .. "/%{cfg.buildcfg}"}
links ({"GraphicsEngine", "MuninGraph", "MuninGraphEditor", "ScriptGraphNodes", "ImGuiNodeEditor"})
print(dirs.tool.."ScriptEditor")
includedirs {dirs.graphicsengine.."../", dirs.external, dirs.utilities, dirs.tool.."FbxSerializer/",dirs.tool, dirs.scripting, dirs.source .. "/DearImGui",  dirs.scripting.."/MuninGraph", dirs.scripting.."/MuninGraphEditor", dirs.scripting.."/ScriptGraphNodes",dirs.scripting.."/ImGuiNodeEditor", dirs.featherEngine}

        files {
            "**.h",
            "**.hpp",
            "**.cpp",
            "*.aps",
            "*.rc"
        }
    
	filter "configurations:Debug"
		defines {"_DEBUG", "WIN32","_LIB"}
		runtime "Debug"
		symbols "on"
		files {"tools/**"}
		includedirs {"tools/"}
		targetname("%{prj.name}_Debug")
	filter "configurations:Release"
		defines {"_RELEASE", "WIN32","_LIB"}
		runtime "Release"
		optimize "on"
		files {"tools/**"}
		includedirs {"tools/"}
		targetname("%{prj.name}")
		flags { 
			"FatalCompileWarnings" 	
		}
	filter "configurations:Retail"
		defines {"_RETAIL", "WIN32","_LIB"}
		runtime "Release"
		optimize "on"
		flags { 
			"FatalCompileWarnings"
		}
	filter "system:windows"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		--links{"d3d11","dxguid", "dxgi", "d3dcompiler" }
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		links({"FeatherEngine","TGAFbx"})
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		