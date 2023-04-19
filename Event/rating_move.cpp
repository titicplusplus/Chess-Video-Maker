#include "rating_move.hpp"


namespace TitiCplusplus {

	RatingMove::RatingMove(float rank, std::size_t currentIndex) : Event{rank, currentIndex} {}
	RatingMove::~RatingMove() {}
	
	const std::string RatingMove::getType() {
		return "RatingMove";
	}

	nlohmann::json RatingMove::getCmd(const std::vector<ChessMove> &cmove) {
		auto defaultType = Ressources::GoodMove;
		
		if (m_rank > -50) {
			if (cmove[m_currentIndex].eatP) {
				defaultType = Ressources::Kill;
			}
		} else {
			if (m_rank < -300) {
				defaultType = Ressources::BugPlayer;
			} else {
				defaultType = Ressources::BadMove;
			}
		}


		return vid.getResVideo(defaultType, cmove, m_currentIndex-1);
	}

}; // TitiCplusplus
