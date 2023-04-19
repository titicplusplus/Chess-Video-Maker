#include "Ressources.hpp"

#include <fstream>

namespace TitiCplusplus {
	nlohmann::json Ressources::m_ressourcesVideo;

	Ressources::Ressources() {
		std::ifstream file {"Ressources/ressources.json"};
		file >> m_ressourcesVideo;
		file.close();

		m_sortRVideo["thinking"] = {};
		m_sortRVideo["sad"] = {};
		m_sortRVideo["waiting"] = {};
		m_sortRVideo["surprise"] = {};
		m_sortRVideo["badmove"] = {};
		m_sortRVideo["movefast"] = {};
		m_sortRVideo["goodmove"] = {};
		m_sortRVideo["kill"] = {};
		m_sortRVideo["angry"] = {};
		m_sortRVideo["bugplayer"] = {};

		for (std::size_t inR {0}; inR < m_ressourcesVideo["video"].size(); ++inR) {
			const nlohmann::json &video { m_ressourcesVideo["video"][inR] };
			if (video["type"].get<std::string>().find("thinking") != std::string::npos) {
				m_sortRVideo["thinking"].push_back(inR);
			}
	
			if (video["type"].get<std::string>().find("sad") != std::string::npos) {
				m_sortRVideo["sad"].push_back(inR);
			}

			if (video["type"].get<std::string>().find("waiting") != std::string::npos) {
				m_sortRVideo["waiting"].push_back(inR);
			}

			if (video["type"].get<std::string>().find("surprise") != std::string::npos) {
				m_sortRVideo["surprise"].push_back(inR);
			}

			if (video["type"].get<std::string>().find("badmove") != std::string::npos) {
				m_sortRVideo["badmove"].push_back(inR);
			}

			if (video["type"].get<std::string>().find("movefast") != std::string::npos) {
				m_sortRVideo["movefast"].push_back(inR);
			}

			if (video["type"].get<std::string>().find("goodmove") != std::string::npos) {
				m_sortRVideo["goodmove"].push_back(inR);
			}

			if (video["type"].get<std::string>().find("kill") != std::string::npos) {
				m_sortRVideo["kill"].push_back(inR);
			}

			if (video["type"].get<std::string>().find("angry") != std::string::npos) {
				m_sortRVideo["angry"].push_back(inR);
			}

			if (video["type"].get<std::string>().find("bugplayer") != std::string::npos) {
				m_sortRVideo["bugplayer"].push_back(inR);
			}

		}
	}

	Ressources::~Ressources() {}

	/**nlohmann::json Ressources::getVideo(RType videoType) {
		return nlohmann::json({});
	}

	nlohmann::json Ressources::getImage(RType imageType) {
		return nlohmann::json({});
	}

	nlohmann::json Ressources::getText(RType textType) {
		return nlohmann::json({});
	}**/

	void Ressources::sortByFind(std::vector<int> &result, std::vector<int> &add) {
		if (result.empty()) {
			result.insert(result.begin(), add.begin(), add.end());
			return;
		}

		for (auto it { result.begin() }; it < result.end();) {
			if (std::find(add.begin(), add.end(), *it) == add.end()) {
				result.erase(it);
			} else {
				it++;
			}
		}
	}

	std::vector<int> Ressources::solution(int mt) {
		std::vector<int> result;

		if (mt & Thinking) {
			result.insert(result.begin(), m_sortRVideo["thinking"].begin(), m_sortRVideo["thinking"].end());
		}

		if (mt & Sad) {
			sortByFind(result, m_sortRVideo["sad"]);
		}

		if (mt & Waiting) {
			sortByFind(result, m_sortRVideo["waiting"]);
		}

		if (mt & Surprise) {
			sortByFind(result, m_sortRVideo["surprise"]);
		}

		if (mt & BadMove) {
			sortByFind(result, m_sortRVideo["badmove"]);
		}

		if (mt & MoveFast) {
			sortByFind(result, m_sortRVideo["movefast"]);
		}

		if (mt & GoodMove) {
			sortByFind(result, m_sortRVideo["goodmove"]);
		}

		if (mt & Win) {
			sortByFind(result, m_sortRVideo["win"]);
		}

		if (mt & Kill) {
			sortByFind(result, m_sortRVideo["kill"]);
		}

		if (mt & Angry) {
			sortByFind(result, m_sortRVideo["angry"]);
		}

		if (mt & BugPlayer) {
			sortByFind(result, m_sortRVideo["bugplayer"]);
		}

		return result;
	}

	nlohmann::json Ressources::getInfoVideo(std::size_t index) {
		return m_ressourcesVideo["video"][index];
	}
}; //TitiCplusplus
