#pragma once
#include <tuple>
#include <array>
#include <string>
#include <memory>
#include <QProcess>

#define MIN_DEPTH 20
#define DEFAULT_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

struct StockFishInfo {
	public:
		int depth = -1;
		int seldepth = -1;
		int multipv = -1;

		std::string pv = {};
		int cp = -1000;
		int mate = -1;

		std::array<int, 3> wdl = {0, 0, 0};

		void draw() const;
};

class StockFishUCI {
	public:
		StockFishUCI(const char* executable,
				std::string_view FEN = "");

		void positionFEN(std::string_view FEN);

		void isReady();
		void setOption(const std::string id, const std::string value);
		std::string go(int depth = MIN_DEPTH);
		std::tuple<int, int, int> getWdlStats(int depth = MIN_DEPTH);
		std::string bestMove(int depth = MIN_DEPTH);

		StockFishInfo processLine(std::string line);

		StockFishInfo information(int depth = MIN_DEPTH);
		std::vector<StockFishInfo> allInformation(int depth = MIN_DEPTH, std::string fen = "");

		~StockFishUCI();
	private:
		QProcess m_uci;
		std::string m_executable;
};


