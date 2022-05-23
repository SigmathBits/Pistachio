#pragma once

#include "pstpch.h"

#include "Core.h"

#include "Events/Event.h"


namespace Pistachio {

	struct WindowProperties {
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProperties(
			const std::string& title = "Pistachio Engine", 
			unsigned int width = 1280, 
			unsigned int height = 720
		) : Title(title), Width(width), Height(height) {}
	};

	// Interface representing a desktop system based window
	class PISTACHIO_API Window {
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int Width() const = 0;
		virtual unsigned int Height() const = 0;

		virtual void SetEventCallback(const EventCallbackFunction& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		static Window* Create(const WindowProperties& properties = WindowProperties());
	};

}
