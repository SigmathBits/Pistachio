include "Dependencies.lua"

workspace "Pistachio"
	architecture "x64"
	startproject "Hat"
	
	configurations {
		"Debug",
		"Release",
		"Dist",
	}

	flags {
		"MultiProcessorCompile",
	}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"


group "Dependencies"
	include "vendor/premake"
	include "Pistachio/vendor/GLFW"
	include "Pistachio/vendor/Glad"
	include "Pistachio/vendor/imgui"
	include "Pistachio/vendor/yaml-cpp"
	include "Pistachio/vendor/Box2D"
group ""

group "Core"
	include "Pistachio"
	include "Pistachio-ScriptCore"
group ""

group "Tools"
	include "Hat"
group ""

group "Miscellaneous"
	include "Sandbox"
group ""
