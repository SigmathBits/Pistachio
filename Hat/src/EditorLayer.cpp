#include "EditorLayer.h"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <ImGuizmo/ImGuizmo.h>

#include "Pistachio/Scene/SceneSerializer.h"

#include "Pistachio/Math/Math.h"
#include "Pistachio/Utils/PlatformUtils.h"


inline std::ostream& operator<<(std::ostream& ostream, const ImVec2& vector) {
	return ostream << "ImVec2(" << vector.x << ", " << vector.y << ")";
}


namespace Pistachio {

	EditorLayer::EditorLayer()
		: Layer("Sandbox2D", { EventType::KeyPressed }, EVENT_CATEGORY_MOUSE_BUTTON), m_EditorCamera(1280, 720, 45.0f, 0.001f, 1000.0f) {
		m_ViewportBounds[0] = { 0.0f, 0.0f };
		m_ViewportBounds[1] = { 0.0f, 0.0f };
	}

	EditorLayer::~EditorLayer() {

	}

	void EditorLayer::OnAttach() {
		PST_PROFILE_FUNCTION();

		m_PistachioTexture = Texture2D::Create("assets/textures/Pistachio.png");
		m_RainbowDashTexture = Texture2D::Create("assets/textures/Rainbow-Dash.png");
		m_CheckerboardTexture = Texture2D::Create("assets/textures/Checkerboard.png");

		FramebufferSpecification frameBufferSpec{ 1280, 720 };
		frameBufferSpec.AttachmentsSpecification = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		m_Framebuffer = Framebuffer::Create(frameBufferSpec);

		m_ActiveScene = CreateRef<Scene>();

		// Load last save
		std::ifstream inFile("assets/scenes/.pistachio_last_save");
		if (inFile) {
			std::string filepath;
			std::getline(inFile, filepath);

			PST_INFO("Loading last save \"{}\" ... ", filepath);

			LoadSceneFile(filepath);
		}

#if 0
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
#endif

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
			unsigned int width = (unsigned int)m_ViewportSize.x;
			unsigned int height = (unsigned int)m_ViewportSize.y;

			m_Framebuffer->Resize(width, height);
			m_EditorCamera.SetViewportSize(width, height);

			m_ActiveScene->OnViewportResize(width, height);
		}

		m_EditorCamera.OnUpdate(timestep);


		// Render
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		RenderCommand::SetClearColour({ 0.03f, 0.10f, 0.15f, 1 });
		RenderCommand::Clear();

		// Clear our Entity ID attachment (index 1) to -1
		m_Framebuffer->ClearColourAttachment(1, -1);

		// Update Scene
		m_ActiveScene->OnUpdateEditor(timestep, m_EditorCamera);

		// Capture which Entity is being hovered over
		auto [x, y] = ImGui::GetMousePos();
		glm::vec2 mousePosition = { x, y };
		mousePosition -= m_ViewportBounds[0];

		int mouseX = (int)mousePosition.x;
		int mouseY = (int)(m_ViewportSize.y - mousePosition.y);

		if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y) {
			int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
			m_HoveredEntity = pixelData == -1 ? Entity{} : Entity{ (entt::entity)pixelData, m_ActiveScene.get() };
		}

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
				if (ImGui::MenuItem("New", "Ctrl+N")) {
					NewScene();
				}

				if (ImGui::MenuItem("Open...", "Ctrl+O")) {
					FileOpen();
				}

				if (ImGui::MenuItem("Save...", "Ctrl+S")) {
					FileSave();
				}

				if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S")) {
					FileSaveAs();
				}

				if (ImGui::MenuItem("Exit", "Ctrl+Q")) {
					Application::Instance().Close();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// Dockspace windows begin
		m_SceneHierarchyPanel.OnImGuiRender();
		m_PropertiesPanel.SetSelectedEntity(m_SceneHierarchyPanel.SelectedEntity());
		m_PropertiesPanel.OnImGuiRender();


		{
			std::string name = "None";
			if (m_HoveredEntity) {
				name = m_HoveredEntity.Component<TagComponent>().Tag;
			}

			auto stats = Renderer2D::RetrieveStats();
			ImGui::Begin("Renderer2D Statistics");
			ImGui::Text("Hovered Entity: %s", name.c_str());
			ImGui::Text("Draw Calls: %d", stats.DrawCalls);
			ImGui::Text("Quad Count: %d", stats.QuadCount);
			ImGui::Text("Vertex Count: %d", stats.TotalVertexCount());
			ImGui::Text("Index Count: %d", stats.TotalIndexCount());
			ImGui::End();
		}

		/// Viewport
		// Display framebuffer to viewport
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });
		ImGui::Begin("Viewport");

		ImVec2 viewportOffset = ImGui::GetCursorPos();  // Includes the tab/title bar
		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		ImVec2 viewportPanelPosition = ImGui::GetWindowPos();

		m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
		m_ViewportBounds[0] = { viewportPanelPosition.x + viewportOffset.x, viewportPanelPosition.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportPanelPosition.x + m_ViewportSize.x, viewportPanelPosition.y + m_ViewportSize.y };

		// Draw Colour Attachment buffer to viewport
		unsigned int colourAttachmentID = m_Framebuffer->ColourAttachmentRendererID(0);
		ImGui::Image((void*)(size_t)colourAttachmentID, viewportPanelSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });


		// Determine when ImGui should block events
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
		auto isAnyItemActive = ImGui::IsAnyItemActive();
		Application::Instance().BaseImGuiLayer()->AllowMouseEventBlocking(m_ViewportIsDragging || !m_ViewportHovered);
		Application::Instance().BaseImGuiLayer()->AllowKeyboardEventBlocking(m_ViewportIsDragging || !m_ViewportFocused && isAnyItemActive);


		// Gizmos
		Entity selectedEntity = m_SceneHierarchyPanel.SelectedEntity();
		if (selectedEntity && m_GizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			// Set our viewport (rectangle)
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportSize.x, m_ViewportSize.y);

			// Runtime Camera projection and view
			//auto cameraEntity = m_ActiveScene->PrimaryCameraEntity();
			//const auto& camera = cameraEntity.Component<CameraComponent>().Camera;

			//glm::mat4 viewMatrix = glm::inverse(cameraEntity.Component<TransformComponent>().Transform());
			//const glm::mat4& projectionMatrix = camera.Projection();

			const glm::mat4& projectionMatrix = m_EditorCamera.ProjectionMatrix();
			const glm::mat4& viewMatrix = m_EditorCamera.ViewMatrix();

			// Entity transform
			auto& entityTransformComponent = selectedEntity.Component<TransformComponent>();
			glm::mat4 transform = entityTransformComponent.Transform();

			// Snapping
			bool snap = Input::IsKeyPressed(PST_KEY_LEFT_CONTROL);
			float snapValue = 0.5f;  // snap to 0.5m for tranlation and scale
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE) {
				snapValue = 45.0f;  // snap to 45 degrees for rotation;
			}
			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(
				glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
				(ImGuizmo::OPERATION)(m_GizmoType), ImGuizmo::LOCAL,
				glm::value_ptr(transform), nullptr, snap ? snapValues : nullptr
			);

			if (m_AllowGizmoInteraction && ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				entityTransformComponent.Translation = translation;
				entityTransformComponent.Rotation = rotation;
				entityTransformComponent.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::End();
	}

	bool EditorLayer::OnEvent(Event& event) {
		m_EditorCamera.SendEvent(event);
		return false;
	}

	bool EditorLayer::OnKeyPressed(Pistachio::KeyPressedEvent& event) {
		// Shortcuts
		if (event.RepeatCount() > 0) return false;

		bool ctrlPressed = Input::IsKeyPressed(PST_KEY_LEFT_CONTROL) || Input::IsKeyPressed(PST_KEY_RIGHT_CONTROL);
		bool shiftPressed = Input::IsKeyPressed(PST_KEY_LEFT_SHIFT) || Input::IsKeyPressed(PST_KEY_RIGHT_SHIFT);

		switch (event.KeyCode()) {
			case PST_KEY_N: {
				if (ctrlPressed) {
					NewScene();
					return true;
				}
				break;
			}
			case PST_KEY_O: {
				if (ctrlPressed) {
					FileOpen();
					return true;
				}
				break;
			}
			case PST_KEY_S: {
				if (ctrlPressed && !shiftPressed) {
					FileSave();
					return true;
				}
				if (ctrlPressed && shiftPressed) {
					FileSaveAs();
					return true;
				}
				break;
			}

			// Gizmos
			case PST_KEY_Q: {
				if (ctrlPressed) {
					Application::Instance().Close();
				}

				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			}
			case PST_KEY_W: {
				m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			}
			case PST_KEY_E: {
				m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
			}
			case PST_KEY_R: {
				if (!Input::IsKeyPressed(PST_KEY_LEFT_ALT)) {
					m_GizmoType = -1;
				}
				break;
			}

			default:
				break;
		}

		return false;
	}

	bool EditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
		if (event.MouseButton() == PST_MOUSE_BUTTON_LEFT && m_ViewportHovered && !ImGuizmo::IsOver()) {
			if (m_GizmoType == -1) {
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			m_PropertiesPanel.SetSelectedEntity(m_SceneHierarchyPanel.SelectedEntity());
		}
		return false;
	}

	void EditorLayer::NewScene() {
		m_ActiveScene = CreateRef<Scene>();
		m_ActiveScene->OnViewportResize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
		m_Filepath = "";
	}

	void EditorLayer::FileOpen() {
		std::string filepath = FileDialog::OpenFile("Pistachio Scene (*.pistachio)\0*.pistachio\0");
		if (!filepath.empty()) {
			NewScene();

			LoadSceneFile(filepath);
		}
	}

	void EditorLayer::FileSave() {
		if (m_Filepath.empty()) {
			FileSaveAs();
			return;
		}

		SaveSceneFile(m_Filepath);
	}

	void EditorLayer::FileSaveAs() {
		std::string filepath = FileDialog::SaveFile("Pistachio Scene (*.pistachio)\0*.pistachio\0");
		if (!filepath.empty()) {
			const std::string filetype = ".pistachio";
			if (filepath.compare(filepath.length() - filetype.length(), filetype.length(), filetype) != 0) {
				filepath = filepath.append(filetype);
			}

			SaveSceneFile(filepath);
		}
	}

	void EditorLayer::LoadSceneFile(std::string& filepath) {
		SceneSerializer serializer(m_ActiveScene);
		serializer.Deserialize(filepath);

		m_Filepath = filepath;
		SetLastSave(filepath);
	}

	void EditorLayer::SaveSceneFile(std::string& filepath) {
		SceneSerializer serializer(m_ActiveScene);
		serializer.Serialize(filepath);

		m_Filepath = filepath;
		SetLastSave(filepath);
	}

	void EditorLayer::SetLastSave(std::string& filepath) {
		std::ofstream outFile("assets/scenes/.pistachio_last_save");
		outFile << filepath;
	}

}
