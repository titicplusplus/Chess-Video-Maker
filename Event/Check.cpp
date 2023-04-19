#include "Check.hpp"
#include "../Ressources.hpp"
#include "../ChessParser.hpp"

namespace TitiCplusplus {

	Check::Check(float rank, std::size_t currentIndex) : Event{rank, currentIndex},
			m_randomSentences{{
				{"You can do anythings", "Are you sure ?", "Yes, always"},
				{"Hello !", "You want to scare me", "", "It's impossible"},
				{"Goodbye, You will die soon", "Not sure, we'll see"},
			}}
	{}

	Check::~Check() {}


	nlohmann::json Check::getCmd(const std::vector<ChessMove> &cmove) {
		unsigned long randomSentence {0};

		do {
 			randomSentence = Default::randomTime(0, 3);
		} while (randomSentence >= m_randomSentences.size());

		std::vector<std::pair<int, int>> positionP;
		positionP.push_back(std::make_pair(cmove[m_currentIndex].newX, 7 - cmove[m_currentIndex].newY));

		int color {int(cmove[m_currentIndex].chessArray[positionP[0].first][cmove[m_currentIndex].newY] / 10)};
		int king { (!color)*10 + 6 };

		positionP.push_back(Default::findPieces(king, cmove[m_currentIndex].chessArray));
		positionP[1].second = 7 - positionP[1].second;

		if (positionP[1].first == -1) {
			std::cerr << "Can't find " << king << '\n';
		}

		return {std::to_string(m_currentIndex-1) + "end", textGeneration.generationDiscussion(m_randomSentences[randomSentence], positionP)};
	}


	const std::string Check::getType() {
		return "Check";
	}

}; // TitiCplusplus
