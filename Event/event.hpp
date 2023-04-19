#pragma once
#include <nlohmann/json.hpp>
#include "../default.hpp"

namespace TitiCplusplus {

class Event {
	public:
		Event(float rank, std::size_t currentIndex);
		~Event();

		const float getRank() const;
		const std::size_t getCurrentIndex() const;
		virtual nlohmann::json getCmd(const std::vector<ChessMove> &cmove) = 0;
		virtual const std::string getType() = 0;

	protected:
		float m_rank;
		std::size_t m_currentIndex;
};

}; // TitiCplusplus
