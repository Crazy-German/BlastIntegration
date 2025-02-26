include "../../../Premake/extensions.lua"
-- include for common stuff 
include "../../../Premake/common.lua"

group "Engine"
include (dirs.graphicsengine)
group "Engine"
include (dirs.audioengine)
group "Engine"
include (dirs.physicsengine)
 group "Engine"
-------------------------------------------------------------
project "FeatherEngine"
location (dirs.projectfiles)
dependson { "GraphicsEngine, GraphicsEngineShader, CommonUtilities, External, Logger, TGAFbx, AudioEngine, SquishPE, MuninGraph"}

kind "StaticLib"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.intermidiate}/%{prj.name}/")
targetdir (dirs.lib.."/%{cfg.buildcfg}")
targetname("%{prj.name}")
objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")
postbuildmessage ("ABANDON ALL HOPE YEE WHO ENTER HERE")

pchheader ("%{prj.name}.pch.h")
pchsource ("%{prj.name}.pch.cpp")
forceincludes ("%{prj.name}.pch.h")
--postbuildcommands {"\"$(VC_ExecutablePath_x64)\\lib.exe\" /out:\"$(SolutionDir)Lib/%{cfg.buildcfg}/%{prj.name}.lib\" \"$(SolutionDir)/Intermediate/%{prj.name}/%{cfg.buildcfg}*.obj\""}
-- pchheader ("%{prj.name}.pch.h")
-- pchsource ("%{prj.name}.pch.cpp")
-- forceincludes ("%{prj.name}.pch.h")
links ({"GraphicsEngine", "External","CommonUtilities", "Logger", "External", "AudioEngine", "SquishPE", "DearImgui"})
print(dirs.featherEngine)
includedirs {dirs.scripting.."/MuninGraph",dirs.graphicsengine.."../", dirs.external, dirs.utilities, dirs.featherEngine, dirs.audioengine.."../", dirs.physicsengine, dirs.tool.."Imgui/", dirs.physicsengine.."Physx/include"}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"*.aps",
		"*.rc"
	}

	libdirs { dirs.lib, dirs.utilities.."TGAFbx/lib/%{cfg.buildcfg}" }
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		files {"tools/**"}
		includedirs {"tools/"}
	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG"}
		runtime "Release"
		optimize "on"
		files {"tools/**"}
		includedirs {"tools/"}
		flags { 
			"FatalCompileWarnings" 	
		}
	filter "configurations:Retail"
		defines {"_RETAIL", "NDEBUG"}
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
		links("TGAFbx")
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		