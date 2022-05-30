workspace "Pistachio"
	architecture "x64"
	startproject "Hat"
	
	configurations {
		"Debug",
		"Release",
		"Dist",
	}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Inlcude directories relative to root folder (Solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Pistachio/vendor/GLFW/include"
IncludeDir["Glad"] = "Pistachio/vendor/Glad/include"
IncludeDir["ImGui"] = "Pistachio/vendor/imgui"
IncludeDir["glm"] = "Pistachio/vendor/glm"
IncludeDir["stb_image"] = "Pistachio/vendor/stb_image"


group "Dependancies"
	include "Pistachio/vendor/GLFW"
	include "Pistachio/vendor/Glad"
	include "Pistachio/vendor/imgui"
group ""


project "Pistachio"
	location "Pistachio"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "pstpch.h"
	pchsource "Pistachio/src/pstpch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.h",
		"%{prj.name}/vendor/stb_image/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.h",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
	}

	links {
		"GLFW",
		"Glad",
		"imgui",
		"opengl32.lib",
	}
	
	defines {
		"_CRT_SECURE_NO_WARNINGS",
	}

	filter "system:windows"
		systemversion "latest"

		defines {
			"PST_PLATFORM_WINDOWS",
			"PST_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

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


project "Hat"
	location "Hat"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediates/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs { 
		"Pistachio/vendor/spdlog/include",
		"Pistachio/src",
		"Pistachio/vendor",
		"%{IncludeDir.glm}",
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
  

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediates/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs { 
		"Pistachio/vendor/spdlog/include",
		"Pistachio/src",
		"Pistachio/vendor",
		"%{IncludeDir.glm}",
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
  