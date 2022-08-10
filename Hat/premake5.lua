project "Hat"
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

	filter "system:linux"
		libdirs {
			"%{LibraryDir.VulkanSDK}",
		}
	
		buildoptions {
			"-fpermissive",
		}

		links {
			"GLFW",
			"Glad",
			"ImGui",
			"yaml-cpp",
			"Box2D",

			"dl",
			"pthread",
			"GL",
			"X11",

			"shaderc_shared",
			"spirv-cross-core",
			"spirv-cross-glsl",
		}


	filter "configurations:Debug"
		defines "PST_DEBUG"
		runtime "Debug"
		symbols "on"

	filter { "configurations:Debug", "system:windows" }
		postbuildcommands {
			"{COPYDIR} %{LibraryDir.VulkanSDK_DebugDLL}/**sharedd.dll \"%{cfg.targetdir}\""
		}

	filter "configurations:Release"
		defines "PST_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "PST_DIST"
		runtime "Release"
		optimize "on"
 
