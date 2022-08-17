local PistachioRootDir = "../../../.."

include (PistachioRootDir .. "/vendor/premake/premake_customisation/solution_items.lua")


workspace "SandboxProj"
	architecture "x64"
	startproject "Sandbox"
	
	configurations {
		"Debug",
		"Release",
		"Dist",
	}
	
	flags {
		"MultiProcessorCompile",
	}


project "Sandbox"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir "Binaries"
	objdir "Intermediates"

	files {
		"Source/**.cs",
		"Properties/**.cs",
	}

	links {
		"Pistachio-ScriptCore",
	}


	filter "configurations:Debug"
		optimize "off"
		symbols "default"

	filter "configurations:Release"
		optimize "on"
		symbols "default"

	filter "configurations:Dist"
		optimize "full"
		symbols "off"


group "Pistachio"
	include (PistachioRootDir .. "/Pistachio-ScriptCore")
group ""
