include "../../../../Premake/extensions.lua"
-- include for common stuff 
include "../../../../Premake/common.lua"
-------------------------------------------------------------
project "ModelViewer"
location (dirs.projectfiles)
dependson { "FeatherEngine, Imgui"}

kind "WindowedApp"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.bin}/%{cfg.buildcfg}")
targetdir ("%{dirs.bin}/%{cfg.buildcfg}")
targetname("%{prj.name}")
objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")

links ({"FeatherEngine"})--, "Imgui","GraphicsEngine", "External","CommonUtilities", "Logger", "External", "AudioEngine", "SquishPE"})
print(dirs.tool)
includedirs {dirs.graphicsengine.."../", dirs.external, dirs.utilities, dirs.featherEngine, dirs.audioengine.."../", dirs.physicsengine, dirs.tool, dirs.tool.."../Imgui/Imgui/"}

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
		links({"FeatherEngine","TGAFbx"})
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		