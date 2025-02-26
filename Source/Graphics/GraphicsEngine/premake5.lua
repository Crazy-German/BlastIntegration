print("GraphicsEngine")
include "../../../Premake/common.lua"
include (dirs.graphicsengineshaders)
include (dirs.external)
group "Utilities"
include (dirs.utilities .. "Logger/")
include (dirs.utilities .. "CommonUtilities/")
group "Engine"
-------------------------------------------------------------
project "GraphicsEngine"
location (dirs.projectfiles)
dependson { "GraphicsEngineShader" , "CommonUtilities", "Logger"}
links({"GraphicsEngineShaders", "CommonUtilities", "Logger"})

kind "StaticLib"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.intermidiate}/%{prj.name}/")
targetdir (dirs.lib.."/%{cfg.buildcfg}")
targetname("%{prj.name}")
objdir (dirs.graphicsengine)


pchheader ("%{prj.name}.pch.h")
pchsource ("%{prj.name}.pch.cpp")
forceincludes ("%{prj.name}.pch.h")

includedirs {dirs.compiledshaders, dirs.utilities, dirs.external, dirs.graphicsengineshaders, dirs.graphicsengine}
	group "Header"
		files {
			"**.h",
			"**.hpp"
		}
	group ""

	group "Source"
		files{
			"**.cpp",
			"*.aps",
			"*.rc"
		}
	group ""
	libdirs { dirs.lib }
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		files {"tools/**"}
		includedirs {"tools/"}
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		files {"tools/**"}
		includedirs {"tools/"}
		flags { 
			--"FatalCompileWarnings"
		}
	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"
		flags { 
			--"FatalCompileWarnings"
		}

	filter "system:windows"
--		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		links{"d3d11","dxguid", "dxgi", "d3dcompiler" }
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		