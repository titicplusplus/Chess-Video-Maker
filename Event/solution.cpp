#include "solution.hpp"
#include "../Ressources.hpp"

namespace TitiCplusplus {

	Solution::Solution(float rank, std::size_t currentIndex) : Event{rank, currentIndex} {}
	Solution::~Solution() {}
	
	const std::string Solution::getType() {
		return "Solution";
	}

	nlohmann::json Solution::getCmd(const std::vector<ChessMove> &cmove) {
		return vid.getResVideo(Ressources::Surprise, cmove, m_currentIndex-1);
	}

}; // TitiCplusplus
