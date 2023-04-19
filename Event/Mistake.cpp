#include "Mistake.hpp"
#include "../Ressources.hpp"

namespace TitiCplusplus {

Mistake::Mistake(float rank, std::size_t currentIndex) : Event{rank, currentIndex} {}

nlohmann::json Mistake::getCmd(const std::vector<ChessMove> &cmove) {
	return vid.getResVideo(Ressources::Sad, cmove, m_currentIndex-1);
};

const std::string Mistake::getType() {
	return "Mistake";
}

}; //TitiCplusplus
