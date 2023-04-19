#pragma once
#include "../Ressources.hpp"
#include "../default.hpp"

namespace TitiCplusplus {

class VideoGeneration {
	Ressources res;

	public:
		nlohmann::json getResVideo(int defaultType, const std::vector<ChessMove> &cmove, std::size_t m_currentIndex);

};

}; // TitiCplusplus


