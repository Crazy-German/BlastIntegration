print("External")
include "../../Premake/common.lua"

-------------------------------------------------------------
project "External"
location (dirs.projectfiles)

kind "StaticLib"
language "C++"
cppdialect "C++20"

targetdir (dirs.lib.."/%{cfg.buildcfg}")
targetname("%{prj.name}")
objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")

includedirs {"rapidjson", "nlohmann", "imnodes", "../DearImGui"}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"*.aps",
		"*.rc"
	}

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
			"FatalCompileWarnings"
		}
	filter "configurations:Retail"
		defines "_RETAIL"
		runtime "Release"
		optimize "on"
		flags { 
			"FatalCompileWarnings"
		}

	filter "system:windows"
--		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		