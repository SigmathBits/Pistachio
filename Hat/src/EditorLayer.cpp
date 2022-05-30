#include "EditorLayer.h"

#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"


inline std::ostream& operator<<(std::ostream& ostream, const ImVec2& vector) {
	return ostream << "ImVec2(" << vector.x << ", " << vector.y << ")";
}


namespace Pistachio {

	EditorLayer::EditorLayer()
		: Layer("Sandbox2D", EVENT_CATEGORY_NONE), m_CameraController(1280, 720, true) {

	}

	EditorLayer::~EditorLayer() {

	}

	void EditorLayer::OnAttach() {
		PST_PROFILE_FUNCTION();

		m_PistachioTexture = Texture2D::Create("assets/textures/Pistachio.png");
		m_RainbowDashTexture = Texture2D::Create("assets/textures/Rainbow-Dash.png");
		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

		FramebufferSpecification frameBufferSpec{ 1280, 720 };
		m_Framebuffer = Framebuffer::Create(frameBufferSpec);
	}

	void EditorLayer::OnDetach() {
		PST_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(Timestep timestep) {
		PST_PROFILE_FUNCTION();

		{
			PST_PROFILE_SCOPE("CameraController.OnUpdate");

			m_CameraController.OnUpdate(timestep);
		}

		// Render
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();
		
		{
			PST_PROFILE_SCOPE("Renderer Screen Clear");


			RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
			RenderCommand::Clear();
		}

		{
			PST_PROFILE_SCOPE("Renderer Scene 1");

			// Squares
			Renderer2D::BeginScene(m_CameraController.Camera());

			Renderer2D::DrawQuad({ { 0.0f, 0.0f, 0.9f }, { 10.0f, 10.0f } }, { m_CheckerboardTexture, { 0.6f, 0.8f, 0.9f, 0.1f }, 10.0f });

			Renderer2D::DrawQuad({ { 0.5f, 0.5f, 0.2f } }, { 1.0f, 0.0f, 1.0f, 1.0f });

			Renderer2D::DrawQuad({ { -0.5f, -0.5f, 0.2f } }, { 0.0f, 1.0f, 0.0f, 1.0f });

			const float stride = 2.0f;
			for (float y = -stride / 2; y < stride / 2; y += 0.1f) {
				for (float x = -stride / 2; x < stride / 2; x += 0.1f) {
					Renderer2D::DrawQuad({ { x, y, 0.1f }, { 0.09f, 0.09f } },
						{ std::abs(x / stride), std::abs(y / stride), 0.0f, 0.8f });
				}
			}

			Renderer2D::EndScene();
		}

		{
			PST_PROFILE_SCOPE("Renderer Scene 2");

			Renderer2D::BeginScene(m_CameraController.Camera());

			Renderer2D::DrawQuad({ { -0.5f, -0.5f, 0.0f }, glm::radians(10.0f) }, { 0.3f, 0.0f, 0.1f, 1.0f });

			Renderer2D::DrawQuad({ { 0.5f, -0.5f, -0.3f }, glm::radians(60.0f), { 0.25f, 0.25f } }, { 0.251f, 0.494f, 0.494f, 0.8f });

			Renderer2D::DrawQuad({ { -0.5f, -0.5f, -0.31f }, glm::radians(-60.0f),{ 0.25f, 0.25f } }, { .251f, 0.494f, 0.494f, 0.5f });

			m_RotatedTransform.Rotation = glm::radians(m_Angle);
			Renderer2D::DrawQuad(m_RotatedTransform, m_Colour);

			static float rotation = 0.0f;
			rotation += 30.0f * timestep;
			Renderer2D::DrawQuad({ { 0.0f, 0.0f, -0.52f }, glm::radians(rotation) }, { m_PistachioTexture, 2.0f });

			Renderer2D::DrawQuad({ { 0.5f, 0.5f, -0.6f } }, m_RainbowDashTexture);

			Renderer2D::EndScene();
		}

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender() {
		PST_PROFILE_FUNCTION();

		// If you strip some features of, this demo is pretty much equivalent to calling DockSpaceOverViewport()!
		// In most cases you should be able to just call DockSpaceOverViewport() and ignore all the code below!
		// In this specific demo, we are not using DockSpaceOverViewport() because:
		// - we allow the host window to be floating/moveable instead of filling the viewport (when opt_fullscreen == false)
		// - we allow the host window to have padding (when opt_padding == true)
		// - we have a local menu bar in the host window (vs. you could use BeginMainMenuBar() + DockSpaceOverViewport() in your code!)
		// TL;DR; this demo is more complicated than what you would normally use.
		// If we removed all the options we are showcasing, this demo would become:
		//     void ShowExampleAppDockSpace()
		//     {
		//         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		//     }

		static bool dockspaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen) {
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		} else {
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Hat DockSpace", &dockspaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("Hat DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) {
					Application::Instance().Close();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		{
			ImGui::Begin("Quad");
			ImGui::DragFloat3("Position", glm::value_ptr(m_RotatedTransform.Position), 0.005f);
			ImGui::DragFloat("Angle", &m_Angle, 1.0f, 0.0f, 360.0f);
			ImGui::DragFloat2("Size", glm::value_ptr(m_RotatedTransform.Size), 0.01f, 0.01f, 10.0f);
			ImGui::ColorEdit4("Colour", glm::value_ptr(m_Colour));
			ImGui::End();

			auto stats = Renderer2D::RetrieveStats();
			ImGui::Begin("Stats");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quad Count: %d", stats.QuadCount);
			ImGui::Text("Vertex Count: %d", stats.TotalVertexCount());
			ImGui::Text("Index Count: %d", stats.TotalIndexCount());
			ImGui::End();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		ImVec2 viewportPanelPosition = ImGui::GetWindowPos();
		glm::vec2 viewportPosition = { viewportPanelPosition.x, viewportPanelPosition.y };
		if (m_ViewportPosition != viewportPosition) {
			m_ViewportPosition = viewportPosition;
			m_IsViewportDragging = Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT);
		}
		if (!Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) {
			m_IsViewportDragging = false;
		}

		m_ViewportHovered = ImGui::IsWindowHovered();
		m_ViewportFocused = ImGui::IsWindowFocused();
		Application::Instance().BaseImGuiLayer()->AllowMouseEventBlocking(m_IsViewportDragging || !m_ViewportHovered);
		Application::Instance().BaseImGuiLayer()->AllowKeyboardEventBlocking(m_IsViewportDragging || !m_ViewportFocused);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		glm::vec2 viewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		if (m_ViewportSize != viewportSize && viewportSize.x > 0 && viewportSize.y > 0) {
			m_Framebuffer->Resize((unsigned int)viewportSize.x, (unsigned int)viewportSize.y);
			m_CameraController.Resize((unsigned int)viewportSize.x, (unsigned int)viewportSize.y);
			m_ViewportSize = viewportSize;
		}

		unsigned int colourAttachmentID = m_Framebuffer->ColourAttachmentRendererID();
		ImGui::Image((void*)colourAttachmentID, viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	bool EditorLayer::OnEvent(Event& event) {
		m_CameraController.SendEvent(event);
		return false;
	}

}
