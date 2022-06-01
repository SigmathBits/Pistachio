project "Pistachio"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "pstpch.h"
	pchsource "src/pstpch.cpp"

	files {
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.h",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/ImGuizmo/ImGuizmo.h",
		"vendor/ImGuizmo/ImGuizmo.cpp",
	}

	includedirs {
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.yaml_cpp}",
		"%{IncludeDir.ImGuizmo}",
	}

	links {
		"GLFW",
		"Glad",
		"imgui",
		"yaml-cpp",
		"opengl32.lib",
	}
	
	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE",
	}

	filter "files:vendor/ImGuizmo/**.cpp"
		flags {
			"NoPCH"
		}

	filter "system:windows"
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
