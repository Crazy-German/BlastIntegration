include "../../Premake/extensions.lua"
-- include for common stuff 
include "../../Premake/common.lua"

workspace "BlastProject-Specialization"
location "../../"
startproject "BlastDemo"
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