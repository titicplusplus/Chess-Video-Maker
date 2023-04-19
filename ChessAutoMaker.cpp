#include "ChessAutoMaker.hpp"
#include "ChessParser.hpp"
#include <memory>
#include <unistd.h>

#include "Event/Check.hpp"
#include "Event/Mistake.hpp"
#include "Event/rating_move.hpp"
#include "Event/multipleAttack.hpp"
#include "Event/solution.hpp"
#include "Event/Mistake.hpp"

using json = nlohmann::json;

AttackInfoP::AttackInfoP(int x, int y, const ChessMove &cmove) {
	this->x = x;
	this->y = y;

	this->piece = cmove.chessArray[x][y];
	this->point = ChessParser::pointOfPiece(piece);
}

constexpr bool colorTest(int color, int x, int y, const ChessMove &cmove) {
	return cmove.chessArray[x][y] != 0 && color != int(cmove.chessArray[x][y]/10);
}

AutoMaker::AutoMaker() : uci {"/mnt/704E048C4E044D72/build/Stockfish/src/stockfish"}
{}


std::vector<std::pair<int, int>> AutoMaker::analyseGame(const nlohmann::json &j) {
	std::vector<std::pair<int, int>> sortRating;

	for (std::size_t i {0}; i < j.size(); i++) {
		if (j[i].contains("rating"))
			sortRating.push_back(std::make_pair(i, j[i]["rating"].get<int>()));
	}

	std::sort(sortRating.begin(), sortRating.end(), [](std::pair<int, int> a, std::pair<int, int> b) {
		return a.second > b.second;
	});

	return sortRating;
}

bool AutoMaker::newModifFile(const std::vector<ChessMove> &cmove, nlohmann::json &j) {
	if (!j.contains("automaker")) {
		j["automaker"] = json::array();
		json &dataAnalyse = j["automaker"];

		std::cout << "Start processing data\n";

		std::string rock {"KQkq "};
		std::string taken = "-";

		for (std::size_t i {0}; i < cmove.size() - 1; i++) {

			std::cout << i << ":" << cmove.size() << '\n';

			if (rock.size() > 2 && i != 0) {// != 0) {
				if (i%2 == 1) {
					if (cmove[i].newX == 9) {
						rock[0] = ' ';
						rock[1] = ' ';
					} else if (cmove[i].oldX == 4 && cmove[i].oldY == 0) {
						rock[0] = ' ';
						rock[1] = ' ';
					}


					if (rock[1] == 'Q') {
						if (cmove[i].oldX == 0 && cmove[i].oldY == 0) {
							rock[0] = ' ';
						}
					}

					if (rock[0] == 'K') {
						if (cmove[i].oldX == 7 && cmove[i].oldY == 0) {
							rock[0] = ' ';
						}
					}
				} else {
					if (cmove[i].newX == 10) {
						rock[2] = ' ';
						rock[3] = ' ';
					} 

					if (rock[2] == 'k') {
						if (cmove[i].oldX == 7 && cmove[i].oldY == 7) {
							rock[2] = ' ';
						} else if (cmove[i].oldX == 4 && cmove[i].oldY == 7) {
							rock[2] = ' ';
							rock[3] = ' ';
						}
					}

					if (rock[3] == 'q') {
						if (cmove[i].oldX == 0 && cmove[i].oldY == 7) {
							rock[3] = ' ';
						}
					}
				}

				if (rock[0] == ' ' && rock[1] == ' ' && rock[2] == ' ' && rock[3] == ' ') {
					rock = "- ";
				}
			}

			if (cmove[i].newX >= 0 && cmove[i].newX < 8 && cmove[i].newY >= 0 && cmove[i].newY < 8) {
				if (cmove[i].chessArray[cmove[i].newX][cmove[i].newY] == 1 && cmove[i].newY == 3 && cmove[i].oldY == 1) {
					taken = "  ";
					taken[0] = ('a' + cmove[i].newX);
					taken[1] = '3';
				} else if (cmove[i].chessArray[cmove[i].newX][cmove[i].newY] == 11 && cmove[i].newY == 4 && cmove[i].oldY == 6) {
					taken[0] = 'a' + cmove[i].newX;
					taken[1] = '6';
				} else {
					taken = "-";
				}
			}

			std::string cFEN {
					ChessParser::chessArraytoFEN(cmove[i].chessArray) + ' ' +
					(i%2 == 0 ? 'w' : 'b') + ' ' + 
					rock + taken + " 0 " +
					std::to_string(int((i)/2) + 1) 
			};

			uci.positionFEN(cFEN);

			std::vector<StockFishInfo> info = uci.allInformation(12, cFEN);
			StockFishInfo &last = info[info.size() - 1];

			std::vector<char> m_value = {char('a' + cmove[i+1].oldX), char('1' + cmove[i+1].oldY),
						char('a' + cmove[i+1].newX), char('1' + cmove[i+1].newY) };

			dataAnalyse.push_back(json({
					{"bestmove", last.pv},
					{"bestmove_cp", last.cp*-(int(i%2)*2 - 1)},//*(int(i%2)*2-1)},
					{"moveplayed", std::string(m_value.begin(), m_value.end())},
					{"fen", cFEN}
			}));
			
			if (i > 0) {
				dataAnalyse[i - 1]["moveplayed_cp"] = last.cp*-(int(i%2)*2 - 1);
				
				//std::cout << i%2 << ' ' << dataAnalyse[i - 1]["bestmove_cp"].get<int>() << ' ' << last.cp << '\n';
				//dataAnalyse[i - 1]["rating"] = (dataAnalyse[i - 1]["bestmove_cp"].get<int>() - last.cp*-(int(i%2)*2 - 1));
				if (i%2) {
					dataAnalyse[i - 1]["rating"] = - last.cp - dataAnalyse[i - 1]["bestmove_cp"].get<int>();
				} else {
					dataAnalyse[i - 1]["rating"] = dataAnalyse[i - 1]["bestmove_cp"].get<int>() - last.cp;
				}
			}

			//ChessParser::draw(cmove[i].chessArray);
		}

		return true;
	}

	return false;
}

std::vector<std::vector<AttackInfoP>> AutoMaker::attackPieces(int pX, int pY, const ChessMove &cmove) {
	const int piece { cmove.chessArray[pX][pY] };

	std::vector<std::vector<AttackInfoP>> finalResult;

	if (piece%10 == 3) {
		for (int attackP {0}; attackP < 4; ++attackP) {
			const int x {pX + ((attackP)%2 + 1)*(int(attackP/2)*2 - 1)};
			int y {pY + ((attackP+1)%2 + 1)};

			if (x >= 0 && x < 8) {
				if (y >= 0 &&  y < 8) {
					if (cmove.chessArray[x][y] != 0 && int(piece/10) != int(cmove.chessArray[x][y]/10)) {
							finalResult.push_back(
								{AttackInfoP{x, y, cmove}}
							);
					}
				}

				y = pY - ((attackP+1)%2 + 1);

				if (y >= 0 &&  y < 8) {
					if (cmove.chessArray[x][y] != 0 && int(piece/10) != int(cmove.chessArray[x][y]/10)) {

						finalResult.push_back(
							{{x, y, cmove}}
						);

					}
				}
			}
		}
	} else if (piece%10 == 1) {
		int adv = -(int(piece/10) - 1);

		const int &pX = cmove.newX + adv;
		const int &pY = cmove.newY;

		if (pX >= 0 && pX < 8) {
			if (pY + 1 < 8 && pY - 1 >= 0) {
				const int p1 {cmove.chessArray[pX][pY + 1]};
				const int p2 {cmove.chessArray[pX][pY - 1]};
				const int color = int(piece/10);

				if (color != int(p1/10) && p1 != 0) {
					finalResult.push_back(
						{{pX, pY + 1, cmove}}
					);
				}

				if (color != int(p2/10) && p1 != 0) {
					finalResult.push_back(
						{{pX, pY - 1, cmove}}
					);
				}
			}
		}
	} else {
		if (piece%10 == 4 || piece%10 == 5) {
			std::size_t currentF {0};

			finalResult.push_back({});
			finalResult.push_back({});
			finalResult.push_back({});
			finalResult.push_back({});
				
			const int color = int(piece/10);

			for (int cpX {pX - 1}; cpX >= 0; cpX--) {
				int cpY { pY + (pX - cpX) };
				if (cpY < 8 && colorTest(color, cpX, cpY, cmove)) {//color != int(cmove.chessArray[cpX][cpY]/10)) {
					finalResult[currentF].push_back({cpX, cpY, cmove});
				} else if (cmove.chessArray[cpX][cpY] > 1) {
					break;
				}
			}

			for (int cpX {pX - 1}; cpX >= 0; cpX--) {
				int cpY = pY - (pX - cpX);
				if (cpY >= 0 && colorTest(color, cpX, cpY, cmove)) {//color != int(cmove.chessArray[cpX][cpY]/10)) {
					finalResult[currentF + 1].push_back({cpX, cpY, cmove});
				} else if (cmove.chessArray[cpX][cpY] > 1) {
					break;
				}
			}

			for (int cpX {pX + 1}; cpX < 8; ++cpX) {
				int cpY { pY + (pX - cpX) };
				if (cpY >= 0 && colorTest(color, cpX, cpY, cmove)) {//color != int(cmove.chessArray[cpX][cpY]/10)) {
					finalResult[currentF + 2].push_back({cpX, cpY, cmove});
				} else if (cmove.chessArray[cpX][cpY] > 1) {
					break;
				}
			}

			for (int cpX {pX + 1}; cpX < 8; ++cpX) {
				int cpY = pY - (pX - cpX);
				if (cpY < 8 && colorTest(color, cpX, cpY, cmove)) {//color != int(cmove.chessArray[cpX][cpY]/10)) {
					finalResult[currentF + 3].push_back({cpX, cpY, cmove});
				} else if (cmove.chessArray[cpX][cpY] > 1) {
					break;
				}
			}
		}

		if (piece%10 == 2 || piece%10 == 5) {
			std::size_t currentF {finalResult.size()};

			finalResult.push_back({});
			finalResult.push_back({});
			finalResult.push_back({});
			finalResult.push_back({});

			const int color { int(piece)/10 };

			for (int cpX {pX - 1}; cpX >= 0; cpX--) {
				if (colorTest(color, cpX, pY, cmove)) {//color != int(cmove.chessArray[cpX][pY]/10)) {
					finalResult[currentF].push_back({cpX, pY, cmove});
				} else if (cmove.chessArray[cpX][pY] > 0) {
					break;
				}
			}

			for (int cpX {pX + 1}; cpX < 8; ++cpX) {
				if (colorTest(color, cpX, pY, cmove)) {//color != int(cmove.chessArray[cpX][pY]/10)) {
					finalResult[currentF+1].push_back({cpX, pY, cmove});
				} else if (cmove.chessArray[cpX][pY] > 0) {
					break;
				}

			}

			for (int cpY {pY - 1}; cpY >= 0; --cpY) {
				if (colorTest(color, pX, cpY, cmove)) {//color != int(cmove.chessArray[pX][cpY]/10)) {
					finalResult[currentF+2].push_back({pX, cpY, cmove});
				} else if (cmove.chessArray[pX][cpY] > 0) {
					break;
				}

			}

			for (int cpY {pY + 1}; cpY < 8; ++cpY) {
				if (colorTest(color, pX, cpY, cmove)) { //color != int(cmove.chessArray[pX][cpY]/10)) {
					finalResult[currentF+3].push_back({pX, cpY, cmove});
				} else if (cmove.chessArray[pX][cpY] > 0) {
					break;
				}

			}
		} else if (piece%10 == 6) {
			const int color { int(piece)/10 };

			for (int cpX {pX - 1}; cpX < 8 && cpX < pX+1; cpX++) {
				if (cpX >= 0) {
					int cpY { pY - 1 };

					if (cpY < 8 && colorTest(color, cpX, cpY, cmove)) {//color != int(cmove.chessArray[cpX][cpY]/10)) {
						finalResult.push_back({{cpX, cpY, cmove}});
					}

					cpY = pY + 1;

					if (cpY < 8 && colorTest(color, cpX, cpY, cmove)) {//color != int(cmove.chessArray[cpX][cpY]/10)) {
						finalResult.push_back({{cpX, cpY, cmove}});
					}
				}
			}

			if (pX - 1 >= 0 && colorTest(color, pX - 1, pY, cmove)) { //int(cmove.chessArray[pX - 1][pY]/10)) {
				finalResult.push_back({{pX - 1, pY, cmove}});
			}

			if (pX + 1 >= 0 && colorTest(color, pX + 1, pY, cmove)) { //int(cmove.chessArray[pX + 1][pY]/10)) {
				finalResult.push_back({{pX + 1, pY, cmove}});
			}
		}
	}

	for (auto it { finalResult.begin() }; it < finalResult.end();) {
		if ((*it).size() == 0) {
			finalResult.erase(it);
		} else {
			it++;
		}
	}

	return finalResult;
}

std::vector<std::unique_ptr<TitiCplusplus::Event>> AutoMaker::makeAnalyse(const std::vector<ChessMove> &cmove, nlohmann::json &j) {
	std::vector<std::unique_ptr<TitiCplusplus::Event>> events;
	std::vector<std::pair<int, int>> sortAnalyse {analyseGame(j["automaker"])}; 

	std::unordered_map<std::string, int> typeEvents {
		{"RatingMove", 0},
		{"MultipleAttack", 0},
		{"Check", 0},
		{"Mistake", 0}
	};
			
	for (std::size_t currentIndex {1}; currentIndex < cmove.size() - 1; currentIndex++) {
		if (cmove[currentIndex].newX < 0 || cmove[currentIndex].newX > 7)
			continue;

		int piece = cmove[currentIndex].chessArray[cmove[currentIndex].newX]
						  [cmove[currentIndex].newY];

		if (cmove[currentIndex].currentX == 1) {
			std::cout << "ui\n";
			events.push_back(
				std::make_unique<TitiCplusplus::Solution>(j["automaker"][currentIndex-1]["rating"], currentIndex));
		} else {
			if (currentIndex < cmove.size() - 2 && cmove[currentIndex+2].eatP && cmove[currentIndex].newX == cmove[currentIndex + 2].oldX
					&& cmove[currentIndex].newY == cmove[currentIndex + 2].oldY) {
				std::vector<std::vector<AttackInfoP>> attackInfo {
					attackPieces(cmove[currentIndex].newX, cmove[currentIndex].newY, cmove[currentIndex]) };

				if (attackInfo.size() > 0) {
					const int &eatX {cmove[currentIndex + 2].newX};
					const int &eatY {cmove[currentIndex + 2].newY};

					const int &advX {cmove[currentIndex + 1].oldX};
					const int &advY {cmove[currentIndex + 1].oldY};

					short pp {0};

					for (std::size_t indexAttackInfo {0}; indexAttackInfo  < attackInfo.size() && pp != 11; ++indexAttackInfo) {
						const std::vector<AttackInfoP> &att {attackInfo[indexAttackInfo]};
						for (std::size_t i {0}; i < att.size(); i++) {
							if (pp < 10) {
								if (att[i].x == eatX && att[i].y == eatY) {
									pp += 10;
								}
							}

							if (pp%10 != 1) {
								if (att[i].x == advX && att[i].y == advY) {
									pp += 1;
								}
							}
						}	
					}

					if (pp == 11) {
						std::vector<std::pair<int, int>> pos = {{eatX, eatY}, {advX, advY}};
						events.push_back(
							std::make_unique<TitiCplusplus::MultipleAttack>(
								j["automaker"][currentIndex-1]["rating"], currentIndex, pos));
						typeEvents["MultipleAttack"]++;
					} /**else if (pp == 10) {
						//// Don't the attack piece => forget
						
						events.push_back(
							std::make_unique<TitiCplusplus::Mistake>(
								j["automaker"][currentIndex]["rating"], currentIndex+1));

						typeEvents["Mistake"]++;
					}**/ else if (!cmove[currentIndex+3].eatP)  {
						std::cout << currentIndex << "ui2\n";
						events.push_back(
							std::make_unique<TitiCplusplus::Mistake>(
								j["automaker"][currentIndex]["rating"], currentIndex+1));

						typeEvents["Mistake"]++;

						//// Move the on it         => mistake
					}
				}
			} else if (cmove[currentIndex].eatP && currentIndex > 1 && cmove[currentIndex].oldX != cmove[currentIndex-2].newX && 
					cmove[currentIndex].oldY != cmove[currentIndex-2].newY &&
					cmove[currentIndex-1].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY] != 0 &&
					ChessParser::pointOfPiece(cmove[currentIndex].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY]) <
					ChessParser::pointOfPiece(cmove[currentIndex-1].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY]) &&
					ChessParser::pointOfPiece(cmove[currentIndex-1].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY]) >
					ChessParser::pointOfPiece(cmove[currentIndex-2].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY])) {

				std::cout << currentIndex << " "
					<< ChessParser::pointOfPiece(cmove[currentIndex].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY]) << " "
					<< ChessParser::pointOfPiece(cmove[currentIndex-1].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY]) << "\n";

				events.push_back(std::make_unique<TitiCplusplus::Mistake>(
					j["automaker"][currentIndex-2]["rating"], currentIndex-1));
					
				typeEvents["Mistake"]++;
			} else if (cmove[currentIndex].eatP && currentIndex > 1 && cmove[currentIndex].oldX != cmove[currentIndex-2].newX && 
					cmove[currentIndex].oldY != cmove[currentIndex-2].newY &&
					cmove[currentIndex+1].newX != cmove[currentIndex].newX && 
					cmove[currentIndex+1].newY != cmove[currentIndex].newY &&
					ChessParser::pointOfPiece(cmove[currentIndex-1].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY]) >
					ChessParser::pointOfPiece(cmove[currentIndex-2].chessArray[cmove[currentIndex].newX][cmove[currentIndex].newY])) {
				std::cout << currentIndex << "ui\n";

				events.push_back(std::make_unique<TitiCplusplus::Mistake>(
					j["automaker"][currentIndex-2]["rating"], currentIndex-1));
				typeEvents["Mistake"]++;
			} else if (cmove[currentIndex].check) {
				events.push_back(std::make_unique<TitiCplusplus::Check>(
					j["automaker"][currentIndex-1]["rating"], currentIndex));
				typeEvents["Check"]++;
			}


		}
	}

	/** double desiredEvent { std::ceil((DESIRED_EVENT - (typeEvents["MultipleAttack"] + typeEvents["Check"]))/2)/sortAnalyse.size() }; **/

	for (std::size_t i {sortAnalyse.size() - 1}; i > static_cast<double>(sortAnalyse.size())*0.85 &&  sortAnalyse[i].second < -100; i--) {
		events.push_back(std::make_unique<TitiCplusplus::RatingMove>(sortAnalyse[i].second, sortAnalyse[i].first+1));
	}

	sortEvent(events, typeEvents);
	return events;
}

void AutoMaker::sortEvent(std::vector<std::unique_ptr<TitiCplusplus::Event>> &events, std::unordered_map<std::string, int> &typeEvents) {
	for (auto it { events.begin() }; it < events.end() - 1;) {
		const std::size_t currentIndex { (*it)->getCurrentIndex() };

		auto itSearch {it + 1};

		for (;itSearch < events.end();) {
			if (currentIndex == (*itSearch)->getCurrentIndex()) {
				if (typeEvents[(*it)->getType()] < typeEvents[(*itSearch)->getType()]) {
					events.erase(itSearch);
					typeEvents[(*itSearch)->getType()]--;
				} else {
					events.erase(it);
					typeEvents[(*it)->getType()]--;
					break;
				}
			} else {
				itSearch++;
			}
		}

		if (itSearch == events.end())
			it++;
	}
}

AutoMaker::~AutoMaker() {}
