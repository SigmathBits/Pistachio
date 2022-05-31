project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/intermediates/" .. outputdir .. "/%{prj.name}")

	files {
		"src/**.h",
		"src/**.cpp",
	}

	includedirs { 
		"%{wks.location}/Pistachio/vendor/spdlog/include",
		"%{wks.location}/Pistachio/src",
		"%{wks.location}/Pistachio/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}",
	}

	links {
		"Pistachio"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "PST_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "PST_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "PST_DIST"
		runtime "Release"
		optimize "on"
