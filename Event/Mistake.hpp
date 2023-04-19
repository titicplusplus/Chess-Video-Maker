#pragma once
#include "event.hpp"
#include "videoGeneration.hpp"

namespace TitiCplusplus {

class Mistake : public Event {
	VideoGeneration vid;

	public:
		Mistake(float rank, std::size_t currentIndex);

		virtual nlohmann::json getCmd(const std::vector<ChessMove> &cmove);
		virtual const std::string getType();
};

}; //TitiCplusplus

