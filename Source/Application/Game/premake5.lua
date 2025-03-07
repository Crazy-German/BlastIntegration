include "../../../Premake/extensions.lua"
-- include for common stuff 
include "../../../Premake/common.lua"
-------------------------------------------------------------
project "BlastDemo"
location (dirs.projectfiles)
dependson { "FeatherEngine", "MuninGraph", "MuninGraphEditor", "ImGuiNodeEditor", "ScriptGraphNodes"}

kind "WindowedApp"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.bin}/")
targetdir ("%{dirs.bin}/")
targetname("%{prj.name}_%{cfg.buildcfg}")
objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")

links ({"FeatherEngine", "MuninGraph", "MuninGraphEditor", "ImGuiNodeEditor", "ScriptGraphNodes"})--, "Imgui","GraphicsEngine", "External","CommonUtilities", "Logger", "External", "AudioEngine", "SquishPE"})
print(dirs.game)
includedirs {dirs.graphicsengine.."../", dirs.external, dirs.utilities, dirs.featherEngine, dirs.audioengine.."../", dirs.physicsengine, dirs.tool, dirs.tool.."../Imgui/Imgui/", dirs.physicsengine.."Physx/include", dirs.scripting, dirs.physicsengine.."Physx/include/Shared/NvFoundation/"}

	files {
		"**.h",
		"**.hpp",
		"**.cpp",
		"*.aps",
		"*.rc"
	}
	
	filter "action:vs*"
	files {"Resources/**"}
	buildaction "ResourceCompile"

	libdirs { dirs.lib, dirs.utilities.."TGAFbx/lib/%{cfg.buildcfg}"}
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		files {"tools/**"}
		includedirs {"tools/"}
		targetname("FM_HPE_102.4_Debug")

	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG"}
		runtime "Release"
		optimize "on"
		targetname("FM_HPE_102.4_Release")

		files {"tools/**"}
		includedirs {"tools/"}
		flags { 
			"FatalCompileWarnings" 	
		}
	filter "configurations:Retail"
		defines {"_RETAIL", "NDEBUG"}
		runtime "Release"
		targetname("FM HPE 102.4")

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
		