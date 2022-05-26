#pragma once

#include "Pistachio/Core.h"

#include "Pistachio/Core/Timestep.h"

#include "Pistachio/Events/EventListener.h"


namespace Pistachio {

	class Layer : public EventListener {
	public:
		Layer(const std::string& name = "Layer") : m_DebugName(name) {}

		Layer(const std::string& name, EventCategory categories) : EventListener({}, categories), m_DebugName(name) {}
		Layer(const std::string& name, std::set<EventType> events, EventCategory categories = EVENT_CATEGORY_ALL) : EventListener(events, categories), m_DebugName(name) {}

		Layer(EventCategory categories) : EventListener({}, categories), m_DebugName("Layer") {}
		Layer(std::set<EventType> events, EventCategory categories = EVENT_CATEGORY_ALL) : EventListener(events, categories), m_DebugName("Layer") {}

		virtual ~Layer() {}

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep timestep) {}

		virtual void OnImGuiRender() {}

		inline const std::string& Name() const { return m_DebugName; }

	protected:
		std::string m_DebugName;

	private:
		EventDispatcher m_EventDispatcher;
	};

}
