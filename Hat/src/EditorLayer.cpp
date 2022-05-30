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

		m_ActiveScene = CreateRef<Scene>();

		m_SquareEntity = m_ActiveScene->CreateEntity("Square");
		m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.7f, 1.0f, 0.6f, 1.0f });

		m_CameraEntity = m_ActiveScene->CreateEntity("Camera");
		m_CameraEntity.AddComponent<CameraComponent>();
		
		m_SecondCameraEntity = m_ActiveScene->CreateEntity("Camera");
		auto& cc = m_SecondCameraEntity.AddComponent<CameraComponent>();
		cc.Primary = false;
	}

	void EditorLayer::OnDetach() {
		PST_PROFILE_FUNCTION();

	}

	void EditorLayer::OnUpdate(Timestep timestep) {
		PST_PROFILE_FUNCTION();

		auto& spec = m_Framebuffer->Specification();
		if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0
			&& (m_ViewportSize.x != spec.Width || m_ViewportSize.y != spec.Height)) {
			m_Framebuffer->Resize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);
			m_CameraController.Resize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);

			m_ActiveScene->OnViewportResize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);
		}

		m_CameraController.OnUpdate(timestep);


		// Render
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		RenderCommand::SetClearColour({ .03f, 0.1f, 0.15f, 1 });
		RenderCommand::Clear();


		Renderer2D::BeginScene(m_CameraController.Camera());

		Pistachio::Renderer2D::DrawQuad({ { 1.0f, 0.0f, 0.1f } }, m_PistachioTexture);

		Renderer2D::EndScene();

		// Update Scene
		m_ActiveScene->OnUpdate(timestep);

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnImGuiRender() {
		PST_PROFILE_FUNCTION();

		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::Begin("Hat DockSpace", nullptr, window_flags);
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
			if (m_SquareEntity) {
				ImGui::Begin("Quad");
				auto& tag = m_SquareEntity.Component<TagComponent>().Tag;
				ImGui::Text("%s", tag.c_str());
				auto& colour = m_SquareEntity.Component<SpriteRendererComponent>().Colour;
				ImGui::ColorEdit4("Colour", glm::value_ptr(colour));
				ImGui::End();
			}

			auto stats = Renderer2D::RetrieveStats();
			ImGui::Begin("Stats");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quad Count: %d", stats.QuadCount);
			ImGui::Text("Vertex Count: %d", stats.TotalVertexCount());
			ImGui::Text("Index Count: %d", stats.TotalIndexCount());
			ImGui::End();
		}

		{
			ImGui::Begin("Camera");
			auto& position = m_CameraEntity.Component<TransformComponent>().Transform[3];
			ImGui::DragFloat3("Camera Transform", glm::value_ptr(position), 0.1f);
			if (ImGui::Checkbox("Primary Camera", &m_UsePrimaryCamera)) {
				m_CameraEntity.Component<CameraComponent>().Primary = m_UsePrimaryCamera;
				m_SecondCameraEntity.Component<CameraComponent>().Primary = !m_UsePrimaryCamera;
			}
			
			{
				auto& camera = m_SecondCameraEntity.Component<CameraComponent>().Camera;
				float orthoSize = camera.OrthographicSize();
				if (ImGui::DragFloat("Second Camera Ortho Size", &orthoSize, 0.1f, 0.1f, 10.0f)) {
					camera.SetOrthographicSize(orthoSize);
				}
			}

			ImGui::End();
		}

		// Display framebuffer to viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		ImVec2 viewportPanelPosition = ImGui::GetWindowPos();
		glm::vec2 viewportPosition = { viewportPanelPosition.x, viewportPanelPosition.y };
		if (m_ViewportPosition != viewportPosition) {
			m_ViewportPosition = viewportPosition;
			m_ViewportIsDragging = Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT);
		}
		if (!Input::IsMouseButtonPressed(PST_MOUSE_BUTTON_LEFT)) {
			m_ViewportIsDragging = false;
		}

		m_ViewportHovered = ImGui::IsWindowHovered();
		m_ViewportFocused = ImGui::IsWindowFocused();
		Application::Instance().BaseImGuiLayer()->AllowMouseEventBlocking(m_ViewportIsDragging || !m_ViewportHovered);
		Application::Instance().BaseImGuiLayer()->AllowKeyboardEventBlocking(m_ViewportIsDragging || !m_ViewportFocused);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

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
