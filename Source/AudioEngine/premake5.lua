print("AudioEngine")
include "../../Premake/common.lua"
-------------------------------------------------------------
project "AudioEngine"
location (dirs.projectfiles)

kind "StaticLib"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.intermidiate}/%{prj.name}/")
targetdir (dirs.lib.."/%{cfg.buildcfg}")
targetname("%{prj.name}")
dependson({"Logger"});
libdirs(dirs.external .. "FMOD/core/lib/x64/", dirs.lib)
links({"fmodL_vc", "Logger"})

includedirs ({dirs.utilities, dirs.audioengine, dirs.external .. "FMOD/core/inc/"})
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
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
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
		