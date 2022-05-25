#pragma once

#include "Pistachio/Core.h"


namespace Pistachio {

	class Texture {
	public:
		virtual ~Texture() = default;

		virtual unsigned int Width() const = 0;
		virtual unsigned int Height() const = 0;

		virtual void Bind(unsigned int slot = 0) const = 0;
	};

	class Texture2D : public Texture {
	public:
		virtual ~Texture2D() = default;

		static Ref<Texture2D> Create(const std::string& filepath);
	};

}