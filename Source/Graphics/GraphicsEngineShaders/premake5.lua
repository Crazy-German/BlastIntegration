print("GraphicsEngineShader")
include "../../../Premake/common.lua"

project "GraphicsEngineShaders"
location (dirs.projectfiles)

kind "ConsoleApp"
language "C++"
cppdialect "C++20"

debugdir ("%{dirs.intermidiate}/%{prj.name}/")
targetdir (path.join(_WORKING_DIR, "Intermediate/Shaders/"));
targetname("{prj.name}")


	files {
		"**.hlsl",
        "**.hlsli"
	}
	filter "files:**.hlsl"
		shaderheaderfileoutput "%{cfg.targetdir}/CompiledShaderHeaders/%{file.basename}.h"
		shaderobjectfileoutput "%{cfg.targetdir}/CompiledShaders/%{file.basename}.cso"
		shadermodel "5.0"
		shadervariablename "BuiltIn_%{file.basename}_ByteCode"
--Get only Vertex Shaders
	filter "files:**VS.hlsl"
		shadertype "Vertex"
--Get only PixelShaders
	filter "files:**PS.hlsl"
       shadertype "Pixel"
--Get only GeometryShaders
	filter "files:**GS.hlsl"
		shadertype "Geometry"
	--Get only ComputeShaders
	filter "files:**CS.hlsl"
		shadertype "Compute"
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
		