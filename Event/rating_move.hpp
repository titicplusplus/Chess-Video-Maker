
#include "event.hpp"
#pragma once
#include "videoGeneration.hpp"
#include "../Ressources.hpp"

namespace TitiCplusplus {

class RatingMove : public Event {
		
	VideoGeneration vid;

	public:
		RatingMove(float rank, std::size_t currentIndex);
		~RatingMove();
		virtual const std::string getType();

		virtual nlohmann::json getCmd(const std::vector<ChessMove> &cmove);
};

}; //TitiCplusplus
