#pragma once

#include <glm/glm.hpp>

#include "Pistachio/Renderer/Texture.h"


namespace Pistachio {

	namespace Utils {

		enum PalleteColour {
			NONE = 0, RED, GREEN, BLUE,
		};


		// Based on the indices of the font in the ImGui fonts array
		enum FontStyle {
			REGULAR = 2, ITALICS = 1, BOLD = 0,
		};


		void ImGuiPushFontStyle(FontStyle fontStyle);

		void ImGuiPushButtonPalleteColour(PalleteColour colour);
		void ImGuiPopButtonPalleteColour();

		bool ImGuiButton(const char* label, const glm::vec2& size = { 0.0f, 0.0f }, PalleteColour colour = NONE, FontStyle fontStyle = REGULAR);
		bool ImGuiButton(const char* label, PalleteColour colour = NONE, FontStyle fontStyle = REGULAR);

		bool ImGuiImageButton(
			Ref<Texture2D> texture, const glm::vec2& size, int framePadding = -1, 
			const glm::vec4& backgroundColour = { 0.0f, 0.0f, 0.0f, 0.0f }, const glm::vec4& tintColour = { 1.0f, 1.0f, 1.0f, 1.0f });

		void ImGuiImage(
			Ref<Texture2D> texture, const glm::vec2& size,
			const glm::vec4& tintColour = { 1.0f, 1.0f, 1.0f, 1.0f }, const glm::vec4& borderColour = { 0.0f, 0.0f, 0.0f, 0.0f });

	}

}