#include "EditorLayer.h"

#include <fstream>
#include <filesystem>

#include <glm/gtc/type_ptr.hpp>

#include <imgui/imgui.h>

#include <ImGuizmo/ImGuizmo.h>

#include "Pistachio/Scene/SceneSerializer.h"

#include "Pistachio/Math/Math.h"

#include "Pistachio/Utils/Utils.h"
#include "Pistachio/Utils/PlatformUtils.h"


inline std::ostream& operator<<(std::ostream& ostream, const ImVec2& vector) {
	return ostream << "ImVec2(" << vector.x << ", " << vector.y << ")";
}


namespace Pistachio {

	static const std::string PistachioSceneFileExtension = ".pistachio";

	EditorLayer::EditorLayer()
		: Layer("Sandbox2D", { EventType::KeyPressed }, EVENT_CATEGORY_MOUSE_BUTTON), m_EditorCamera(1280, 720, 45.0f, 0.001f, 1000.0f),
		m_ContentBrowserPanel("assets") {
		m_ViewportBounds[0] = { 0.0f, 0.0f };
		m_ViewportBounds[1] = { 0.0f, 0.0f };
	}

	EditorLayer::~EditorLayer() {

	}

	void EditorLayer::OnAttach() {
		PST_PROFILE_FUNCTION();
		

		// Editor Resources
		m_PlayIcon = Texture2D::Create("resources/icons/toolbar/play.png", 9);
		m_SimulateIcon = Texture2D::Create("resources/icons/toolbar/simulate.png", 9);
		m_StopIcon = Texture2D::Create("resources/icons/toolbar/stop.png", 9);


		FramebufferSpecification framebufferSpec{ 1280, 720 };
		framebufferSpec.AttachmentsSpecification = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		m_Framebuffer = Framebuffer::Create(framebufferSpec);


		Renderer2D::SetLineThickness(3.0f);


		m_EditorScene = CreateRef<Scene>();

		// Load save
		auto args = Application::Instance().Specification().Arguments;
		std::string sceneFilepath = "";
		if (args.Count > 1) {
			sceneFilepath = args[1];
		} else {
			// Load last save
			std::ifstream inFile("assets/scenes/.pistachio_last_save");
			if (inFile) {
				std::getline(inFile, sceneFilepath);
			}
		}

		if (!sceneFilepath.empty() && std::filesystem::exists(sceneFilepath)) {
			PST_INFO("Loading save \"{}\" ... ", sceneFilepath);
			LoadSceneFile(sceneFilepath);
		} else {
			SetWindowTitle(m_EditorScene->Name(), true);
		}

		ChangeActiveSceneTo(m_EditorScene);
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


		// Render
		Renderer2D::ResetStats();

		m_Framebuffer->Bind();

		RenderCommand::SetClearColour({ 0.03f, 0.10f, 0.15f, 1 });
		RenderCommand::Clear();

		// Clear our Entity ID attachment (index 1) to -1
		m_Framebuffer->ClearColourAttachment(1, -1);

		// Update Scene
		switch (m_SceneState) {
			case SceneState::Edit: {
				m_EditorCamera.OnUpdate(timestep);

				m_ActiveScene->OnUpdateEditor(timestep, m_EditorCamera);
				break;
			}
			case SceneState::Play: {
				m_ActiveScene->OnUpdateRuntime(timestep);
				break;
			}
			case SceneState::Simulate: {
				m_EditorCamera.OnUpdate(timestep);

				m_ActiveScene->OnUpdateSimulation(timestep, m_EditorCamera);
				break;
			}
		}


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

		OnOverlayRender();

		m_Framebuffer->Unbind();
	}

	void EditorLayer::OnOverlayRender() {
		switch (m_SceneState) {
			case SceneState::Edit:
			{
				Renderer2D::BeginScene(m_EditorCamera);
				break;
			}
			case SceneState::Play:
			{
				Entity entity = m_ActiveScene->PrimaryCameraEntity();
				if (!entity) return;
				Renderer2D::BeginScene(entity.Component<CameraComponent>().Camera, entity.Component<TransformComponent>().Transform());
				break;
			}
		}
		

		if (m_ShowPhysicsColliders) {
			m_ActiveScene->EachEntityWith<BoxCollider2DComponent>([](Entity entity) {
				auto& transformComponent = entity.Component<TransformComponent>();
				auto& boxCollider = entity.Component<BoxCollider2DComponent>();

				glm::vec3 scale = transformComponent.Scale * glm::vec3(boxCollider.Size, 1.0f);

				// Require this order to correctly replicate where the box colider is positioned
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformComponent.Translation);
				transform = glm::rotate(transform, transformComponent.Rotation.z, { 0.0f, 0.0f, 1.0f });
				transform = glm::translate(transform, glm::vec3(boxCollider.Offset, 0.01f));
				transform = glm::scale(transform, scale);

				Renderer2D::DrawRect(transform, { 0.0f, 0.792f, 0.969f, 1.0f });
			});

			m_ActiveScene->EachEntityWith<CircleCollider2DComponent>([](Entity entity) {
				auto& transformComponent = entity.Component<TransformComponent>();
				auto& circleCollider = entity.Component<CircleCollider2DComponent>();

				glm::vec3 scale = glm::vec3(transformComponent.Scale.x * 2.0f * glm::vec2(circleCollider.Radius), 1.0f);  // Only respects x-axis scale

				// Require this order to correctly replicate where the circle colider is positioned
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), transformComponent.Translation);
				transform = glm::rotate(transform, transformComponent.Rotation.z, { 0.0f, 0.0f, 1.0f });
				transform = glm::translate(transform, glm::vec3(circleCollider.Offset, 0.01f));
				transform = glm::scale(transform, scale);

				Renderer2D::DrawCircle(transform, { 0.0f, 0.792f, 0.969f, 1.0f }, 0.05f);
			});
		}


		if (Entity selectedEntity = m_SceneHierarchyPanel.SelectedEntity()) {
			const auto& transformComponent = selectedEntity.Component<TransformComponent>();
			Renderer2D::DrawRect(transformComponent.Transform(), { 0.804f, 0.914f, 0.29f, 1.0f });
		}


		Renderer2D::EndScene();
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
					FileNew();
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

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
				std::string path = (const char*)payload->Data;

				if (Utils::EndsWith(path, PistachioSceneFileExtension)) {
					LoadSceneFile(std::string(path));
				}
			}
			ImGui::EndDragDropTarget();
		}


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
		if (selectedEntity && m_GizmoType != -1 && m_SceneState == SceneState::Edit) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			// Set our viewport (rectangle)
			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportSize.x, m_ViewportSize.y);

			// Runtime Camera projection and view
			//auto cameraEntity = m_ActiveScene->PrimaryCameraEntity();
			//const auto& camera = cameraEntity.Component<CameraComponent>().Camera;
			
			//glm::mat4 viewMatrix = glm::inverse(cameraEntity.Component<TransformComponent>().Transform());
			//const glm::mat4& projectionMatrix = camera.ProjectionMatrix();

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

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				entityTransformComponent.Translation = translation;
				entityTransformComponent.Rotation = rotation;
				entityTransformComponent.Scale = scale;
			}
		}

		ImGui::End();
		ImGui::PopStyleVar();

		/// Dockspace windows begin
		m_SceneHierarchyPanel.OnImGuiRender();
		m_PropertiesPanel.SetSelectedEntity(m_SceneHierarchyPanel.SelectedEntity());
		m_PropertiesPanel.OnImGuiRender();

		m_ContentBrowserPanel.OnImGuiRender();
		

		{
			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Physics Colliders", &m_ShowPhysicsColliders);
			ImGui::End();
		}


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

		UIToolbar();

		ImGui::End();
	}

	void EditorLayer::UIToolbar() {
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 2.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, { 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_Button, { 0.0f, 0.0f, 0.0f, 0.0f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.0f, 0.0f, 0.0f, 0.0f });
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.0f, 0.0f, 0.0f, 0.0f });

		ImGui::Begin("##Toolbar", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);


		float size = ImGui::GetWindowHeight() - 4.0f;
		ImGui::SetCursorPosX(0.5f * ImGui::GetWindowContentRegionMax().x - 0.5f * size);


		// Play button
		{
			Ref<Texture2D> icon = m_SceneState != SceneState::Play ? m_PlayIcon : m_StopIcon;

			if (ImGui::ImageButton((ImTextureID)icon->RendererID(), { size, size }, { 0, 1 }, { 1, 0 }, 0)) {
				switch (m_SceneState) {
					case SceneState::Simulate:
					case SceneState::Edit:
						OnScenePlay();
						break;
					case SceneState::Play:
						OnSceneStop();
						break;
					default:
						PST_ASSERT(false, "Invalid Scene State transition");
						break;
				}
			}
		}

		ImGui::SameLine();

		// Simulate button
		{
			Ref<Texture2D> icon = m_SceneState != SceneState::Simulate ? m_SimulateIcon : m_StopIcon;

			if (ImGui::ImageButton((ImTextureID)icon->RendererID(), { size, size }, { 0, 1 }, { 1, 0 }, 0)) {
				switch (m_SceneState) {
					case SceneState::Play:
					case SceneState::Edit:
						OnSceneSimulationPlay();
						break;
					case SceneState::Simulate:
						OnSceneStop();
						break;
					default:
						PST_ASSERT(false, "Invalid Scene State transition");
						break;
				}
			}
		}

		ImGui::End();

		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor();
	}

	void EditorLayer::OnScenePlay() {
		if (m_SceneState == SceneState::Simulate) {
			OnSceneStop();
		}

		m_SceneState = SceneState::Play;

		m_RuntimeScene = Scene::Copy(m_EditorScene);

		m_RuntimeScene->OnRuntimeStart();

		ChangeActiveSceneTo(m_RuntimeScene);
	}

	void EditorLayer::OnSceneStop() {
		switch (m_SceneState) {
			case SceneState::Play:
				m_RuntimeScene->OnRuntimeStop();
				break;
			case SceneState::Simulate:
				m_RuntimeScene->OnSimulationStop();
				break;
			default:
				PST_ASSERT(false, "Invalid playing Scene State");
				break;
		}

		ChangeActiveSceneTo(m_EditorScene);

		m_RuntimeScene = nullptr;

		m_SceneState = SceneState::Edit;
	}

	void EditorLayer::OnSceneSimulationPlay() {
		if (m_SceneState == SceneState::Play) {
			OnSceneStop();
		}

		m_SceneState = SceneState::Simulate;

		m_RuntimeScene = Scene::Copy(m_EditorScene);

		m_RuntimeScene->OnSimulationStart();

		ChangeActiveSceneTo(m_RuntimeScene);
	}

	bool EditorLayer::OnEvent(Event& event) {
		m_ContentBrowserPanel.SendEvent(event);
		return false;
	}

	bool EditorLayer::OnEventAfter(Event& event) {
		if (m_SceneState == SceneState::Play) return false;
		m_EditorCamera.SendEvent(event);
		return false;
	}

	bool EditorLayer::OnKeyPressed(Pistachio::KeyPressedEvent& event) {
		// Shortcuts
		if (event.IsRepeated()) return false;

		bool ctrlPressed = Input::IsKeyPressed(PST_KEY_LEFT_CONTROL) || Input::IsKeyPressed(PST_KEY_RIGHT_CONTROL);
		bool shiftPressed = Input::IsKeyPressed(PST_KEY_LEFT_SHIFT) || Input::IsKeyPressed(PST_KEY_RIGHT_SHIFT);

		switch (event.KeyCode()) {
			// Contextual Escape actions
			case PST_KEY_ESCAPE: {
				if (m_GizmoType != -1) {
					m_GizmoType = -1;
					return true;
				} else if (m_SceneHierarchyPanel.SelectedEntity()) {
					m_SceneHierarchyPanel.SetSelectedEntity({});
					m_PropertiesPanel.SetSelectedEntity({});
					return true;
				}
				break;
			}

			// File Menu shortcuts
			case PST_KEY_N: {
				if (ctrlPressed) {
					FileNew();
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
		}

		if (m_SceneState != SceneState::Edit) return false;

		switch (event.KeyCode()) {
			// Commands
			case PST_KEY_D: {
				Entity selectedEntity = m_SceneHierarchyPanel.SelectedEntity();
				if (selectedEntity && ctrlPressed) {
					m_EditorScene->DuplicateEntity(selectedEntity);
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

	bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& event) {
		if (m_SceneState == SceneState::Play) return false;

		if (m_HoveredEntity && event.MouseButton() == PST_MOUSE_BUTTON_MIDDLE && Input::IsKeyPressed(PST_KEY_LEFT_CONTROL)) {
			const auto& transformComponent = m_HoveredEntity.Component<TransformComponent>();
			m_EditorCamera.SetFocalPlane(transformComponent.Translation);
		}
		return false;
	}

	bool EditorLayer::OnMouseButtonReleased(MouseButtonReleasedEvent& event) {
		if (m_ViewportHovered && event.MouseButton() == PST_MOUSE_BUTTON_LEFT
			&& (m_GizmoType == -1 || !ImGuizmo::IsOver() || !m_SceneHierarchyPanel.SelectedEntity())) {
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
			m_PropertiesPanel.SetSelectedEntity(m_HoveredEntity);

			if (m_SceneState == SceneState::Play || !m_HoveredEntity) return false;

			if (m_GizmoType == -1) {
				m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
			}

			if (Input::IsKeyPressed(PST_KEY_LEFT_CONTROL)) {
				const auto& transformComponent = m_HoveredEntity.Component<TransformComponent>();
				m_EditorCamera.SetFocalPoint(transformComponent.Translation);
			}
		}
		return false;
	}

	void EditorLayer::FileNew() {
		ChangeSceneTo(CreateRef<Scene>());

		SetWindowTitle(m_EditorScene->Name(), true);

		m_Filepath = "";
	}

	void EditorLayer::FileOpen() {
		std::string filepath = FileDialog::OpenFile("Pistachio Scene (*.pistachio)\0*.pistachio\0");
		if (!filepath.empty()) {
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
			if (!Utils::EndsWith(filepath, PistachioSceneFileExtension)) {
				filepath = filepath.append(PistachioSceneFileExtension);
			}

			SaveSceneFile(filepath);
		}
	}

	void EditorLayer::LoadSceneFile(const std::filesystem::path& filepath) {
		// Currenty likely to throw exception if bad file is passed in
		Ref<Scene> newScene = CreateRef<Scene>();

		SceneSerializer serializer(newScene);
		bool success = serializer.Deserialize(filepath);

		if (!success) return;

		ChangeSceneTo(newScene);

		// Needs to be updated in case the file was renamed
		m_EditorScene->SetName(filepath.filename().string());

		SetWindowTitle(m_EditorScene->Name());

		m_Filepath = filepath;
		SetLastSave(filepath);
	}

	void EditorLayer::SaveSceneFile(const std::filesystem::path& filepath) {
		m_EditorScene->SetName(filepath.filename().string());

		SceneSerializer serializer(m_EditorScene);
		serializer.Serialize(filepath);

		SetWindowTitle(m_EditorScene->Name(), false);

		m_Filepath = filepath;
		SetLastSave(filepath);
	}

	void EditorLayer::SetLastSave(const std::filesystem::path& filepath) {
		std::ofstream outFile("assets/scenes/.pistachio_last_save");
		outFile << filepath.string();
	}

	void EditorLayer::ChangeSceneTo(Ref<Scene> scene) {
		if (m_SceneState != SceneState::Edit) {
			OnSceneStop();
		}

		m_EditorScene = scene;
		m_EditorScene->OnViewportResize((unsigned int)m_ViewportSize.x, (unsigned int)m_ViewportSize.y);

		ChangeActiveSceneTo(m_EditorScene);
	}

	void EditorLayer::ChangeActiveSceneTo(Ref<Scene> scene) {
		m_HoveredEntity = {};
		m_SceneHierarchyPanel.SetSelectedEntity({});
		m_PropertiesPanel.SetSelectedEntity({});

		m_ActiveScene = scene;
		m_SceneHierarchyPanel.SetContext(m_ActiveScene);
	}

	void EditorLayer::SetWindowTitle(const std::string& title, bool unsavedChanges /*= false*/) {
		Application::Instance().CurrentWindow().SetTitle("Hat - " + title + (unsavedChanges ? "*" : ""));
	}

}
