#pragma once


namespace Pistachio {

	class Platform {
	public:
		static float Time();

	protected:
		virtual float TimeImpl() const = 0;

	private:
		static Platform* m_Platform;
	};

}
