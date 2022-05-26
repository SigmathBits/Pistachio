#include "pstpch.h"

#include "ImGuiLayer.h"

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "Pistachio/Application.h"

#include "Platform/Windows/WindowsWindow.h"

#include "Pistachio/KeyCodes.h"


namespace Pistachio {

	ImGuiLayer::ImGuiLayer() 
		: Layer("ImGui", EVENT_CATEGORY_NONE) {

	}

	ImGuiLayer::~ImGuiLayer() {

	}

	void ImGuiLayer::OnAttach() {
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGuiStylesPsistachio();
		ImGuiStyleColoursPistachio();

		// Don't know how to properly load files which are only local to Pistachio
		io.Fonts->AddFontFromFileTTF("../Pistachio/assets/fonts/GidoleFont/Gidole-Regular.ttf", 16.0f);

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Instance();
		GLFWwindow* nativeWindow = static_cast<GLFWwindow*>(app.CurrentWindow().NativeWindow());

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(nativeWindow, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach() {
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender() {

	}

	void ImGuiLayer::Begin() {
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End() {
		ImGuiIO& io = ImGui::GetIO();

		Application& app = Application::Instance();
		Window& window = app.CurrentWindow();
		io.DisplaySize = ImVec2((float)window.Width(), (float)window.Height());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnEvent(Event& event) {
		ImGuiIO& io = ImGui::GetIO();
		if (event.IsInCategory(EVENT_CATEGORY_INPUT) && (io.WantCaptureMouse || io.WantCaptureKeyboard)) {
			event.Handled = true;
		}
	}

	void ImGuiLayer::ImGuiStylesPsistachio() {
		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowBorderSize = 0.0f;

		style.WindowRounding = 5.0f;
		style.ChildRounding =  5.0f;
		style.PopupRounding =  5.0f;
		style.FrameRounding =  3.0f;
		style.GrabRounding =   3.0f;

		style.WindowMenuButtonPosition = ImGuiDir_Right;
	}

	void ImGuiLayer::ImGuiStyleColoursPistachio() {
		// Fallback style
		ImGui::StyleColorsClassic();

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(0.99f, 0.98f, 0.82f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.64f, 0.63f, 0.51f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.12f, 0.13f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.04f, 0.09f, 0.10f, 0.90f);
		colors[ImGuiCol_Border] = ImVec4(0.35f, 0.44f, 0.51f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.07f, 0.22f, 0.25f, 0.90f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.49f, 0.49f, 0.50f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.48f, 0.55f, 0.66f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.31f, 0.45f, 0.15f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.34f, 0.53f, 0.11f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.31f, 0.45f, 0.15f, 0.67f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.57f, 0.74f, 0.38f, 0.81f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.07f, 0.22f, 0.25f, 0.50f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.49f, 0.49f, 0.32f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.49f, 0.49f, 0.52f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.25f, 0.49f, 0.49f, 0.72f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.96f, 0.98f, 0.71f, 0.83f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.99f, 0.98f, 0.82f, 0.41f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 0.97f, 0.62f, 0.76f);
		colors[ImGuiCol_Button] = ImVec4(0.25f, 0.49f, 0.49f, 0.78f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.49f, 0.49f, 0.59f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.37f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.53f, 0.80f, 0.16f, 0.40f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.53f, 0.80f, 0.16f, 0.70f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.80f, 0.16f, 0.83f);
		colors[ImGuiCol_Separator] = ImVec4(0.37f, 0.58f, 0.61f, 0.52f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.51f, 0.69f, 0.76f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.67f, 0.84f, 0.88f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.59f, 0.87f, 0.93f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.59f, 0.87f, 0.93f, 0.39f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.13f, 0.36f, 0.40f, 0.89f);
		colors[ImGuiCol_Tab] = ImVec4(0.49f, 0.69f, 0.25f, 0.95f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.80f, 0.91f, 0.29f, 0.94f);
		colors[ImGuiCol_TabActive] = ImVec4(0.56f, 0.72f, 0.00f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.40f, 0.68f, 0.09f, 0.59f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.64f, 0.85f, 0.05f, 0.59f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.77f, 0.75f, 0.48f, 0.75f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.80f, 0.91f, 0.29f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.80f, 0.91f, 0.29f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.49f, 0.69f, 0.25f, 1.00f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.31f, 0.45f, 0.15f, 1.00f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.19f, 0.45f, 0.45f, 1.00f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.09f, 0.29f, 0.33f, 1.00f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.04f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.58f, 0.85f, 1.00f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(0.84f, 0.95f, 0.30f, 0.91f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.96f, 0.99f, 0.71f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
	}

}
