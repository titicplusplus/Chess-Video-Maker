#pragma once
#include <nlohmann/json.hpp>
#include <vector>
#include "../default.hpp"
#define DEFAULT_CAM_SIZE 0.5

namespace TitiCplusplus {

class TextGeneration {
	public:
		nlohmann::json generationDiscussion(const std::vector<std::string> &m_randomSentences, const std::vector<std::pair<int, int>> &positionP);
};

}; // TitiCplusplus
