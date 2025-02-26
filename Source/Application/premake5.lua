include "../../Premake/extensions.lua"
-- include for common stuff 
include "../../Premake/common.lua"

workspace "P7"
location "../../"
startproject "FM_HPE_102.4"
architecture "x64"

configurations {
	"Debug",
	"Release",
	"Retail"
}
print("Scripting")

group("Scripting")
include(dirs.scripting)
print("Engine")
group "Engine"
include(dirs.featherEngine)
group("")
print("Game")
include(dirs.game)
group("Tools")
include(dirs.app.."Tools/")
include(dirs.source.."DearImgui/")
print("Tools")
--include(dirs.app.."PrefabEditor")