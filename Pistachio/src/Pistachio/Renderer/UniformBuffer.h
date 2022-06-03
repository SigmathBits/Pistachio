#pragma once

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	class UniformBuffer {
	public:
		virtual ~UniformBuffer() {}

		virtual void SetData(const void* data, size_t size, size_t offset = 0) = 0;

		static Ref<UniformBuffer> Create(size_t size, unsigned int binding);
	};

}
