#pragma once
#include "event.hpp"
#include "textGeneration.hpp"
#include <vector>

namespace TitiCplusplus {

class MultipleAttack : public Event {
	public:
		MultipleAttack(float rank, std::size_t currentIndex, std::vector<std::pair<int, int>> &pieces);
		~MultipleAttack();

		virtual const std::string getType();
		virtual nlohmann::json getCmd(const std::vector<ChessMove> &cmove);
	private:
		std::vector<std::pair<int, int>> m_pieces;

		const std::vector<std::string> m_attackSentences;
		const std::vector<std::string> m_defenceSentences;
		const std::vector<std::string> m_anwserSentences;

		TextGeneration textGeneration;

};

}; //TitiCplusplus
