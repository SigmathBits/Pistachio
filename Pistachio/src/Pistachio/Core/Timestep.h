#pragma once


namespace Pistachio {

	class Timestep {
	public:
		Timestep(float seconds = 0.0f)
			: m_TimeSeconds(seconds) {}

		float Seconds() const { return m_TimeSeconds; }
		float Milliseconds() const { return 1000.0f * m_TimeSeconds; }

		operator float() const { return m_TimeSeconds; }

	private:
		float m_TimeSeconds;
	};

}
