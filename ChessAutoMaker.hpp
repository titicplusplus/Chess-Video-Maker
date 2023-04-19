#pragma once
#include <mutex>
#include <nlohmann/json.hpp>
#include "StockFishUCI.hpp"
#include "default.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
#include "Event/event.hpp"
#define DESIRED_EVENT 15


class AttackInfoP {
	public:
		int x;
		int y;
		int piece;
		int point;

		AttackInfoP(int x, int y, const ChessMove &cmove);
};

class AutoMaker {
	public:
		AutoMaker();
		~AutoMaker();

		bool newModifFile(const std::vector<ChessMove> &cmove, nlohmann::json &j);
		std::vector<std::unique_ptr<TitiCplusplus::Event>> makeAnalyse(const std::vector<ChessMove> &cmove, nlohmann::json &j);
		std::vector<std::pair<int, int>> analyseGame(const nlohmann::json &j);
		std::vector<std::vector<AttackInfoP>> attackPieces(int pX, int pY, const ChessMove &cmove);

		void sortEvent(std::vector<std::unique_ptr<TitiCplusplus::Event>> &events, std::unordered_map<std::string, int> &typeEvents);

	private:
		StockFishUCI uci;
};
