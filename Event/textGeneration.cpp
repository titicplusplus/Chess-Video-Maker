#include "textGeneration.hpp"

namespace TitiCplusplus {

	nlohmann::json TextGeneration::generationDiscussion(const std::vector<std::string> &m_randomSentences,
			const std::vector<std::pair<int, int>> &positionP) {
		nlohmann::json output = nlohmann::json::array();

		output.push_back({
				{"fcam", true}, 
				{"cam", {0.5, positionP[0].first, 0.5, positionP[0].second, 0.5, DEFAULT_CAM_SIZE}},
				{"time", 20}}
		);
	

		float cam_size { DEFAULT_CAM_SIZE };

		std::size_t ppIndex { 0 };
		for (std::size_t i {0};i < m_randomSentences.size();) { //i++
			//ppIndex = i%(positionP.size());
			//
		
			if (m_randomSentences[i] == "") {
				std::cout << "ERROROROR " << i << " " << m_randomSentences.size() << " " << positionP.size() << "\n";
				for (const auto &ui : m_randomSentences) {
					std::cout << " " << ui << "\n";
				}

				std::cout << std::endl;
			}
		
			output.push_back({
				{"fcam", true},
				{"cam", {positionP[ppIndex].first, positionP[ppIndex].second, cam_size}},
				{"time", "Effect"},
				{"sound", {"Effect", 0.5}},
				{"Voice", m_randomSentences[i]},
			});

			do {
				i++;
			} while (i < m_randomSentences.size() && m_randomSentences[i] == "");

			const std::size_t ppIndexNew = i%(positionP.size());
			if (i < m_randomSentences.size()) {
				output.push_back({
						{"fcam", true},
						{"cam", {positionP[ppIndex].first, positionP[ppIndexNew].first, 
							 positionP[ppIndex].second, positionP[ppIndexNew].second,
							 DEFAULT_CAM_SIZE, DEFAULT_CAM_SIZE}},
						{"time", 5},
				});
			}
			ppIndex = ppIndexNew;
		}

		output.push_back({
				{"fcam", true}, 
				{"cam", {positionP[ppIndex].first, 0.5, positionP[ppIndex].second, 0.5, cam_size, 0.5}},
				{"time", 20}}
		);

		return output;

	}

}; // TitiCplusplus
