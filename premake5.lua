workspace "Pistachio"
	architecture "x64"
	
	configurations {
		"Debug",
		"Release",
		"Dist",
	}


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Pistachio"
	location "Pistachio"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediates/" .. outputdir .. "/%{prj.name}")

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs {
		"%{prj.name}/vendor/spdlog/include",
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
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
		symbols "On"

	filter "configurations:Debug"
		defines "PST_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "PST_DIST"
		optimize "On"

	filter { "system:windows", "configurations:Release" }
		buildoptions "/MT"  -- Set multi-threaded flag


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
		"Pistachio/src"
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
  