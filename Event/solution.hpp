#pragma once
#include "videoGeneration.hpp"
#include "event.hpp"

namespace TitiCplusplus {

class Solution : public Event {
	VideoGeneration vid;
	public:
		Solution(float rank, std::size_t currentIndex);
		~Solution();
		virtual const std::string getType();

		virtual nlohmann::json getCmd(const std::vector<ChessMove> &cmove);
};

}; //TitiCplusplus
