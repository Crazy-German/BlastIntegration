----------------------------------------------------------------------------
-- the dirs table is a listing of absolute paths, since we generate projects
-- and files it makes a lot of sense to make them absolute to avoid problems
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
dirs = {}
dirs["root"] 			= os.realpath("../")
dirs["bin"]				= os.realpath(dirs.root .. "Bin/")
dirs["temp"]			= os.realpath(dirs.root .. "Temp/")
dirs["lib"]				= os.realpath(dirs.root .. "Lib/")
dirs["projectfiles"]	= os.realpath(dirs.root .. "Local/")
dirs["source"] 			= os.realpath(dirs.root .. "Source/")
dirs["intermidiate"] 	= os.realpath(dirs.root .. "Intermediate/")
dirs["app"] 			= os.realpath(dirs.root .. "Source/Application/")
dirs["tool"] 			= os.realpath(dirs.root .. "Source/Application/Tools/")
dirs["featherEngine"] 	= os.realpath(dirs.root .. "Source/Application/FeatherEngine/")
dirs["game"] 			= os.realpath(dirs.root .. "Source/Application/Game/")
dirs["external"]		= os.realpath(dirs.root .. "Source/External/")
dirs["audioengine"] 	= os.realpath(dirs.root .. "Source/AudioEngine/")
dirs["physicsengine"] 	= os.realpath(dirs.root .. "Source/Squish/")
dirs["graphicsengine"]			= os.realpath(dirs.root .. "Source/Graphics/GraphicsEngine/")
dirs["graphicsengineshaders"] = os.realpath(dirs.root .. "Source/Graphics/GraphicsEngineShaders/")
dirs["utilities"] = os.realpath(dirs.root .. "Source/Utilities/")
dirs["settings"]		= os.realpath(dirs.root .. "Bin/settings/")
dirs["assets"] 	= os.realpath(dirs.root .. "Bin/Assets")
dirs["compiledshaders"] = os.realpath(dirs.root .. "Intermediate/Shaders/")
dirs["scripting"] = os.realpath(dirs.root .. "Source/Scripting/")

dirs["shaders"]	= {}
dirs.shaders["absolute"] = os.realpath(dirs.root .. "Bin/Shaders/")
dirs.shaders["relative"] = "../Bin/Shaders/"

engine_settings = os.realpath(dirs.settings .. "/EngineSettings.json")

if not os.isdir (dirs.bin) then
	os.mkdir (dirs.bin)
	os.mkdir (dirs.assets)
end

if not os.isdir(dirs.settings) then 
	os.mkdir (dirs.settings)
end

if not os.isdir(dirs.projectfiles) then
	os.mkdir (dirs.projectfiles)
end