#include "pstpch.h"
#include "ImGuiUtils.h"

#include <imgui.h>

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	namespace Utils {

		void ImGuiPushFontStyle(FontStyle fontStyle) {
			ImGuiIO& io = ImGui::GetIO();
			ImFont* font = io.Fonts->Fonts[fontStyle];
			ImGui::PushFont(font);
		}

		void ImGuiPushButtonPalleteColour(PalleteColour colour) {
			switch (colour) {
				case NONE:
					break;
				case RED:
					ImGui::PushStyleColor(ImGuiCol_Button,	      ImVec4{ 0.83f, 0.36f, 0.27f, 1.00f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.97f, 0.54f, 0.30f, 1.00f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.93f, 0.33f, 0.30f, 1.00f });
					break;
				case GREEN:
					ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.40f, 0.60f, 0.20f, 1.00f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.75f, 0.85f, 0.20f, 1.00f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.49f, 0.69f, 0.26f, 1.00f });
					break;
				case BLUE:
					ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4{ 0.23f, 0.62f, 0.68f, 1.00f });
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.40f, 0.78f, 0.88f, 1.00f });
					ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4{ 0.15f, 0.71f, 0.82f, 1.00f });
					break;
				default:
					PST_CORE_ASSERT(false, "Unrecognised PalleteColour");
					break;
			}
		}

		void ImGuiPopButtonPalleteColour() {
			ImGui::PopStyleColor(3);
		}

		bool ImGuiButton(const char* label, const glm::vec2& size, PalleteColour colour, FontStyle fontStyle) {
			ImGuiPushButtonPalleteColour(colour);
			ImGuiPushFontStyle(fontStyle);

			bool result = ImGui::Button(label, { size.x, size.y });

			ImGui::PopFont();

			if (colour != NONE) {
				ImGuiPopButtonPalleteColour();
			}

			return result;
		}

		bool ImGuiButton(const char* label, PalleteColour colour, FontStyle fontStyle) {
			return ImGuiButton(label, { 0.0f, 0.0f }, colour, fontStyle);
		}

		bool ImGuiImageButton(
			Ref<Texture2D> texture, const glm::vec2& size, int framePadding,
			const glm::vec4& backgroundColour, const glm::vec4& tintColour) {

			bool result = ImGui::ImageButton(
				(ImTextureID)texture->RendererID(),
				{ size.x, size.y }, 
				{ 0.0f, 1.0f }, { 1.0f, 0.0f }, 
				framePadding, 
				{ backgroundColour.x, backgroundColour.y, backgroundColour.z, backgroundColour.w },
				{ tintColour.x, tintColour.y, tintColour.z, tintColour.w }
			);
			
			return result;
		}

		void ImGuiImage(
			Ref<Texture2D> texture, const glm::vec2& size,
			const glm::vec4& tintColour /*= { 1.0f, 1.0f, 1.0f, 1.0f }*/, const glm::vec4& borderColour /*= { 0.0f, 0.0f, 0.0f, 0.0f }*/) {

			ImGui::Image(
				(ImTextureID)texture->RendererID(),
				{ size.x, size.y },
				{ 0.0f, 1.0f }, { 1.0f, 0.0f },
				{ tintColour.x, tintColour.y, tintColour.z, tintColour.w },
				{ borderColour.x, borderColour.y, borderColour.z, borderColour.w }
			);
		}

	}

}

