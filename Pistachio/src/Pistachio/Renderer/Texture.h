#pragma once

#include "Pistachio/Core/Core.h"


namespace Pistachio {

	class Texture {
	public:
		virtual ~Texture() = default;

		virtual unsigned int Width() const = 0;
		virtual unsigned int Height() const = 0;

		virtual unsigned int RendererID() const = 0;

		virtual void SetData(void* data, size_t size) = 0;

		virtual void Bind(unsigned int slot = 0) const = 0;

		virtual std::string ResourceLocation() const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture {
	public:
		virtual ~Texture2D() = default;

		static Ref<Texture2D> Create(unsigned int width, unsigned int height);
		static Ref<Texture2D> Create(const std::string& filepath, unsigned int levels = 1);
	};

}
