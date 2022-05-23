workspace "Pistachio"
	architecture "x64"
	
	configurations {
		"Debug",
		"Release",
		"Dist",
	}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Inlcude directories relative to root folder (solition directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Pistachio/vendor/GLFW/include"
IncludeDir["Glad"] = "Pistachio/vendor/Glad/include"


include "Pistachio/vendor/GLFW"
include "Pistachio/vendor/Glad"


project "Pistachio"
	location "Pistachio"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "pstpch.h"
	pchsource "Pistachio/src/pstpch.cpp"

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
	}

	links {
		"GLFW",
		"Glad",
		"opengl32.lib",
	}

	filter "system:windows"
		cppdialect "C++17"
		--staticruntime "on"
		systemversion "latest"

		defines {
			"PST_PLATFORM_WINDOWS",
			"PST_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "PST_DEBUG"
		runtime "Debug"
		--buildoptions "/MDd"
		symbols "on"

	filter "configurations:Release"
		defines "PST_RELEASE"
		runtime "Release"
		--buildoptions "/MD"
		optimize "on"

	filter "configurations:Dist"
		defines "PST_DIST"
		runtime "Release"
		--buildoptions "/MD"
		optimize "on"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
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
	}

	links {
		"Pistachio"
	}

	filter "system:windows"
		cppdialect "C++17"
		--staticruntime "on"
		systemversion "latest"

		defines {
			"PST_PLATFORM_WINDOWS",
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


workspace "Pistachio"
  architecture "x64"
  startproject "Sandbox"
  