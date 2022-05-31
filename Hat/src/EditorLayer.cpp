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

		class RandomColour : public ScriptableEntity {
		public:
			void OnCreate() {
				auto& sprite = Component<SpriteRendererComponent>();

				srand(static_cast<unsigned int>(time(0)));
				auto random = []() { return static_cast<float>(rand()) / static_cast<float>(RAND_MAX); };
				sprite.Sprite.TintColour = { random(), random(), random(), 1.0f};
			}
		};

		Entity pistachioEntity = m_ActiveScene->CreateEntity("Pistachio");
		pistachioEntity.AddComponent<SpriteRendererComponent>(m_PistachioTexture);
		pistachioEntity.AddComponent<NativeScriptComponent>().Bind<RandomColour>();

		Entity rainbowDashEntity = m_ActiveScene->CreateEntity("Rainbow Dash");
		rainbowDashEntity.AddComponent<SpriteRendererComponent>(m_RainbowDashTexture);
		rainbowDashEntity.Component<TransformComponent>().Translation = { 1.5f, 0.0f, 0.0f };

		Entity primaryCameraEntity = m_ActiveScene->CreateEntity("Primary Camera");
		primaryCameraEntity.AddComponent<CameraComponent>(5.0f);
		
		Entity secondCameraEntity = m_ActiveScene->CreateEntity("Secondary Camera");
		auto& cc = secondCameraEntity.AddComponent<CameraComponent>();
		cc.Primary = false;

		class CameraController : public ScriptableEntity {
		public:
			void OnCreate() {
				PST_TRACE("CameraController::OnCreate");
			}

			void OnDestroy() {}

			void OnUpdate(Timestep timestep) {
				auto& transform = Component<TransformComponent>();
				auto& camera = Component<CameraComponent>();

				if (!camera.Primary) return;

				constexpr float speed = 5.0f;
				if (Input::IsKeyPressed(PST_KEY_A)) {
					transform.Translation.x -= speed * timestep;
				}
				if (Input::IsKeyPressed(PST_KEY_D)) {
					transform.Translation.x += speed * timestep;
				}
				if (Input::IsKeyPressed(PST_KEY_S)) {
					transform.Translation.y -= speed * timestep;
				}
				if (Input::IsKeyPressed(PST_KEY_W)) {
					transform.Translation.y += speed * timestep;
				}
			}
		};

		primaryCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();
		secondCameraEntity.AddComponent<NativeScriptComponent>().Bind<CameraController>();

		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
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

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Hat DockSpace", nullptr, window_flags);
		ImGui::PopStyleVar(3);

		// Submit the DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float windowMinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 370.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
			ImGuiID dockspace_id = ImGui::GetID("Hat DockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = windowMinSizeX;

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Exit")) {
					Application::Instance().Close();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// Dockspace windows begin
		m_SceneHierarchyPanel.OnImGuiRender();

		{
			auto stats = Renderer2D::RetrieveStats();
			ImGui::Begin("Renderer2D Statistics");
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quad Count: %d", stats.QuadCount);
			ImGui::Text("Vertex Count: %d", stats.TotalVertexCount());
			ImGui::Text("Index Count: %d", stats.TotalIndexCount());
			ImGui::End();
		}

		// Display framebuffer to viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		unsigned int colourAttachmentID = m_Framebuffer->ColourAttachmentRendererID();
		ImGui::Image((void*)(size_t)colourAttachmentID, viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		// Determine when ImGui should block events
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

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	bool EditorLayer::OnEvent(Event& event) {
		m_CameraController.SendEvent(event);
		return false;
	}

}
