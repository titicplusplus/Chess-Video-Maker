#include <nlohmann/json.hpp>
#include <unordered_map>
#pragma once

namespace TitiCplusplus {

class Ressources {
	static nlohmann::json m_ressourcesVideo;
	std::unordered_map<std::string, std::vector<int>>  m_sortRVideo;

	public:
		enum {
			Thinking = 0,
    			Sad = 1 << 0,
    			Waiting = 1 << 2, 
    			Surprise = 1 << 3, 
    			BadMove = 1 << 4, 
    			MoveFast = 1 << 5, 
    			GoodMove =  1 << 6, 
    			Win = 1 << 7, 
    			Kill = 1 << 8, 
    			Angry = 1 << 9, 
    			BugPlayer = 1 << 10, 
		};

		Ressources();
		~Ressources();

		void sortByFind(std::vector<int> &result, std::vector<int> &add);
		nlohmann::json getInfoVideo(std::size_t index);

		std::vector<int> solution(int mt);
};

}; // TitiCplusplus
