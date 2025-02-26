include "../../../Premake/extensions.lua"
-- include for common stuff 
include "../../../Premake/common.lua"

-- workspace "P5"
-- location "../../../"
-- startproject "FeatherEngine"
-- architecture "x64"

-- configurations {
-- 	"Debug",
-- 	"Release",
-- 	"Retail"
-- }


include (dirs.graphicsengine)
include (dirs.audioengine)
include (dirs.physicsengine)
-------------------------------------------------------------
project "PrefabEditor"
location (dirs.projectfiles)
dependson { "GraphicsEngine, GraphicsEngineShader, CommonUtilities, External, Logger, TGAFbx, AudioEngine, SquishPE"}

kind "WindowedApp"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.bin}/%{cfg.buildcfg}")
targetdir ("%{dirs.bin}/%{cfg.buildcfg}")
targetname("%{prj.name}")
objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")
postbuildmessage ("ABANDON ALL HOPE YEE WHO ENTER HERE")

postbuildcommands {"\"$(VC_ExecutablePath_x64)\\lib.exe\" /out:\"$(SolutionDir)Lib/%{cfg.buildcfg}/%{prj.name}.lib\" \"$(SolutionDir)/Intermediate/%{prj.name}/%{cfg.buildcfg}*.obj\""}

links ({"GraphicsEngine", "External","CommonUtilities", "Logger", "External", "AudioEngine", "SquishPE", "Imgui"})
print(dirs.featherEngine)
includedirs {dirs.graphicsengine.."../", dirs.external, dirs.utilities, dirs.featherEngine, dirs.audioengine.."../", dirs.physicsengine, dirs.tool.."Imgui/"}

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
		