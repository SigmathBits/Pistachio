#pragma once

#include <xhash>


namespace Pistachio {

	class UUID {
	public:
		UUID();
		UUID(uint64_t id);
		UUID(const UUID&) = default;

		operator uint64_t() const { return m_UUID;  }

	private:
		uint64_t m_UUID;
	};

}


namespace std {

	template<>
	struct hash<Pistachio::UUID> {
		std::size_t operator()(const Pistachio::UUID& uuid) const {
			return (uint64_t)uuid;
		}
	};

}
