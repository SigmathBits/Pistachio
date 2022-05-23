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


include "Pistachio/vendor/GLFW"


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
	}

	links {
		"GLFW",
		"opengl32.lib",
	}

	filter "system:windows"
		cppdialect "C++17"
		--staticruntime "on"
		systemversion "latest"

		defines {
			"PST_PLATFORM_WINDOWS",
			"PST_BUILD_DLL",
		}

		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "PST_DEBUG"
		--buildoptions "/MDd"
		runtime "Debug"
		symbols "on"

	filter "configurations:Debug"
		defines "PST_RELEASE"
		--buildoptions "/MD"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		--buildoptions "/MD"
		defines "PST_DIST"
		runtime "Release"
		optimize "on"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

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
		staticruntime "On"
		systemversion "latest"

		defines {
			"PST_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "PST_DEBUG"
		symbols "On"

	filter "configurations:Debug"
		defines "PST_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "PST_DIST"
		optimize "On"


workspace "Pistachio"
  architecture "x64"
  startproject "Sandbox"
  