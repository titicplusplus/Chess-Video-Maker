#include "multipleAttack.hpp"
#include <cstddef>

namespace TitiCplusplus {

	MultipleAttack::MultipleAttack(float rank, std::size_t currentIndex, std::vector<std::pair<int, int>> &pieces) :
		Event{rank, currentIndex} ,
		m_attackSentences {"You have no choice, die or survive", "Hi, this is the end for you", "See you later !"},
		m_defenceSentences {"Indeed, I have only one choice", "It is time for me to leave", "All things come to an end"},
		m_anwserSentences {"No !", "Bye, I'm sorry !", "Please, don't go"}
		
	{
		m_pieces.push_back(std::make_pair(-1, -1));
		m_pieces.insert(m_pieces.end(), pieces.begin(), pieces.end());
	}

	MultipleAttack::~MultipleAttack() {}

	const std::string MultipleAttack::getType() {
		return "MultipleAttack";
	}

	nlohmann::json MultipleAttack::getCmd(const std::vector<ChessMove> &cmove) {
		if (m_pieces[0].first == -1) {
			m_pieces[0] = std::make_pair(cmove[m_currentIndex].newX, 7 - cmove[m_currentIndex].newY);

			for (std::size_t index {1}; index < m_pieces.size(); index++) {
				m_pieces[index].second = 7 - m_pieces[index].second;
			}

			if (m_currentIndex+2 < cmove.size() && cmove[m_currentIndex+2].eatP) {
				if (cmove[m_currentIndex].newX == cmove[m_currentIndex+2].oldX && 
					cmove[m_currentIndex].newY == cmove[m_currentIndex+2].oldY) {

					for (std::size_t index {2}; index < m_pieces.size(); index++) {
						if (m_pieces[index].first == cmove[m_currentIndex+2].newX &&
								m_pieces[index].second == 7 - cmove[m_currentIndex+2].newY) {
							std::swap(m_pieces[1], m_pieces[index]);
							break;
						}
					}
				}
			}
		}

		std::vector<std::string> goText {m_attackSentences[Default::randomTime(0, m_attackSentences.size())],
						m_defenceSentences[Default::randomTime(0, m_defenceSentences.size())]};

		while (goText.size() != m_pieces.size()) {
			goText.push_back(m_anwserSentences[Default::randomTime(0, m_anwserSentences.size())]);
		}
		
		return {std::to_string(m_currentIndex-1) + "end", textGeneration.generationDiscussion(goText, m_pieces)};
	}

}; // TitiCplusplus
