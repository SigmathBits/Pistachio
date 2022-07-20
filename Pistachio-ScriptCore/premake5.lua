project "Pistachio-ScriptCore"
	kind "SharedLib"
	language "C#"
	dotnetframework "4.7.2"

	targetdir "%{wks.location}/Hat/Resources/Scripts"
	objdir "%{wks.location}/Hat/Resources/Scripts/Intermediates"

	files {
		"Source/**.cs",
		"Properties/**.cs",
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"
