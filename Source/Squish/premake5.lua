print("Squish Physics")
include "../../Premake/common.lua"
-------------------------------------------------------------
project "SquishPE"
location (dirs.projectfiles)

kind "StaticLib"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.intermidiate}/%{prj.name}/")
targetdir (dirs.lib.."/%{cfg.buildcfg}")
targetname("%{prj.name}")

includedirs ({dirs.utilities, dirs.physicsengine, dirs.physicsengine.."Colliders", dirs.physicsengine.."Physx", dirs.physicsengine.."Physics", dirs.physicsengine.."Physx/include", 
	dirs.physicsengine.."Physx/include/shared/NvFoundation", dirs.physicsengine.."Physx/include/lowlevel", dirs.physicsengine.."Physx/include/globals", dirs.physicsengine.."Physx/include/extensions",
	dirs.physicsengine.."Physx/include/extensions/authoringCommon", dirs.physicsengine.."Physx/include/extensions/authoring"})
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
	libdirs ({dirs.lib, dirs.physicsengine.."Physx/lib/%{cfg.buildcfg}", dirs.physicsengine.."Physx/lib/%{cfg.buildcfg}/Blast"})
	local files = os.matchfiles(dirs.physicsengine.."Physx/lib/Debug/*.lib");
    local libs = table.translate(files, path.getname)
	links(libs)
	local files = os.matchfiles(dirs.physicsengine.."Physx/lib/Debug/Blast/*.lib");
    local libs = table.translate(files, path.getname)
	links(libs)
	defines {"PX_PHYSX_STATIC_LIB"}

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines {"_RELEASE", "NDEBUG"}
		runtime "Release"
		optimize "on"
		flags { 
			"FatalCompileWarnings"
		}
	filter "configurations:Retail"
		defines {"_RETAIL",	"NDEBUG"}
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
		