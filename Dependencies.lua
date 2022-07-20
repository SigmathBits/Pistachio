-- Pistachio Dependencies

-- VULKAN_SDK install location
VULKAN_SDK = os.getenv("VULKAN_SDK")


-- Inlcude directories relative to root folder (Solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/Pistachio/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Pistachio/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Pistachio/vendor/imgui"
IncludeDir["glm"] = "%{wks.location}/Pistachio/vendor/glm"
IncludeDir["stb_image"] = "%{wks.location}/Pistachio/vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Pistachio/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/Pistachio/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/Pistachio/vendor/ImGuizmo"
IncludeDir["Box2D"] = "%{wks.location}/Pistachio/vendor/Box2D/include"
IncludeDir["mono"] = "%{wks.location}/Pistachio/vendor/mono/include"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"


LibraryDir = {}

LibraryDir["mono"] = "%{wks.location}/Pistachio/vendor/mono/lib/%{cfg.buildcfg}"

-- VulkanSDK Library locations
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_DLL"] = "%{VULKAN_SDK}/Bin"
-- These are located  in the same place as the VulkanSDK for v1.3.204.1 and later
LibraryDir["VulkanSDK_Debug"] = LibraryDir["VulkanSDK"]
LibraryDir["VulkanSDK_DebugDLL"] = LibraryDir["VulkanSDK_DLL"]



Library = {}

Library["mono"] = "%{LibraryDir.mono}/libmono-static-sgen.lib"

-- VulkanSDK library files
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"


-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMultimedia"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["WinBCrypt"] = "Bcrypt.lib"
