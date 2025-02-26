include "../../Premake/extensions.lua"

-- include for common stuff 
include "../../Premake/common.lua"


group "Scripting"	
-- Scripting Projects
project "ImGuiNodeEditor"
    location (dirs.scripting .. "/ImGuiNodeEditor")
	targetdir (dirs.lib.."/%{cfg.buildcfg}")
	debugdir ("%{dirs.intermidiate}/%{prj.name}/")

    -- uuid "A4912CDB-DF6B-46C5-8B09-4D3DDB85F985"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
    includedirs {dirs.scripting .. "/ImGuiNodeEditor", dirs.source .."/DearImGui"}

	files {
		dirs.scripting .. "/ImGuiNodeEditor/**.h",
		dirs.scripting .. "/ImGuiNodeEditor/**.hpp",
		dirs.scripting .. "/ImGuiNodeEditor/**.cpp"
	}

	filter "configurations:Debug"
		defines {"_DEBUG","_LIB"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG","_LIB"}
		runtime "Release"
		optimize "on"
	filter "configurations:Retail"
		defines {"_RETAIL", "NDEBUG","_LIB"}
		runtime "Release"
		optimize "on"

	filter "system:windows"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}

project "MuninGraph"
	prebuildmessage ("Kill Me Please")
    location (dirs.scripting .. "/MuninGraph")
	targetdir (dirs.lib.."/%{cfg.buildcfg}")
	debugdir ("%{dirs.intermidiate}/%{prj.name}/")
    -- uuid "CAB443DD-87C5-4989-A385-F45201559698"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
	includedirs {dirs.scripting .. "/MuninGraph", dirs.utilities}
	
	files {
		dirs.scripting .. "/MuninGraph/**.h",
		dirs.scripting .. "/MuninGraph/**.hpp",
		dirs.scripting .. "/MuninGraph/**.cpp"
	}

	filter "configurations:Debug"
		defines {"_DEBUG","_LIB"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG","_LIB"}
		runtime "Release"
		optimize "on"
	filter "configurations:Retail"
		defines {"_RETAIL", "NDEBUG","_LIB"}
		runtime "Release"
		optimize "on"

	filter "system:windows"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}

project "MuninGraphEditor"
	
	pchheader "pch.h"
	pchsource "%{prj.name}/pch.cpp"
	forceincludes ("pch.h")
   	
	location (dirs.scripting .. "/MuninGraphEditor")
	targetdir (dirs.lib.."/%{cfg.buildcfg}")
	debugdir ("%{dirs.intermidiate}/%{prj.name}/")

    -- uuid "3996B64C-9A2B-41F5-9D0E-1A1525DAE621"
    kind "StaticLib"
    language "C++"
	cppdialect "C++20"
    includedirs {dirs.scripting .. "/MuninGraphEditor", dirs.scripting .. "/MuninGraph", dirs.source .."/DearImGui", dirs.featherEngine, dirs.utilities}
	files {
		dirs.scripting .. "/MuninGraphEditor/**.h",
		dirs.scripting .. "/MuninGraphEditor/**.hpp",
		dirs.scripting .. "/MuninGraphEditor/**.cpp"
		
	}
	
	filter { "files:**/DDSTextureLoader11.cpp" }
		flags { "ExcludeFromBuild", "NoPCH"}

	filter "configurations:Debug"
		defines {"_DEBUG","_LIB"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG","_LIB"}
		runtime "Release"
		optimize "on"
	filter "configurations:Retail"
		defines {"_RETAIL", "NDEBUG","_LIB"}
		runtime "Release"
		optimize "on"

	filter "system:windows"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}

	
	

project "ScriptGraphNodes"

	location (dirs.scripting .. "/ScriptGraphNodes")
	targetdir (dirs.lib.."/%{cfg.buildcfg}")
	debugdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")
	objdir (dirs.intermidiate .. "%{prj.name}/%{cfg.buildcfg}")
	-- uuid "1B06105B-C006-49EC-8500-983773169CB7"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	includedirs {"../External",dirs.scripting .. "/ScriptGraphNodes", dirs.scripting .. "/MuninGraph", dirs.utilities, dirs.featherEngine}
	
	pchheader "pch.h"
	pchsource "%{prj.name}/pch.cpp"

	forceincludes ("pch.h")

	files {
		dirs.scripting .. "/ScriptGraphNodes/**.h",
		dirs.scripting .. "/ScriptGraphNodes/**.hpp",
		dirs.scripting .. "/ScriptGraphNodes/**.cpp"
	}

	filter "configurations:Debug"
		defines {"_DEBUG","_LIB"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG","_LIB"}
		runtime "Release"
		optimize "on"
	filter "configurations:Retail"
		defines {"_RETAIL", "NDEBUG","_LIB"}
		runtime "Release"
		optimize "on"
	filter "system:windows"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			-- "FatalCompileWarnings",
			"MultiProcessorCompile"
		}
group ""