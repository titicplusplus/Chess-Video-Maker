#pragma once
#include "event.hpp"
#include "textGeneration.hpp"

#define DEFAULT_CAM_SIZE 0.5

namespace TitiCplusplus {

class Check : public Event {
	public:
		Check(float rank, std::size_t currentIndex);
		~Check();

		virtual nlohmann::json getCmd(const std::vector<ChessMove> &cmove);
		virtual const std::string getType();

	private:
		const std::array<std::vector<std::string>, 3> m_randomSentences;

		TextGeneration textGeneration;
};

}; //TitiCplusplus

