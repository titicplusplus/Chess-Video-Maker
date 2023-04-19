#include "ChessParser.hpp"
#include <iostream>

/**struct ChessMove {
	int newX;
	int newY;
	int oldX;
	int oldY;
	bool eatP;
	std::array<std::array<int, 8>, 8> chessArray;
};***/

using json = nlohmann::json;

ChessParser::ChessParser() {
	reset();
}


int ChessParser::pointOfPiece(int piece) {
	piece = piece%10;

	if (piece == 0) {
		return 0;
	} else if (piece == 1) {
		return 1;
	} else if (piece == 2) {
		return 5;
	} else if (piece == 3 || piece == 4) {
		return 3;
	} else {
		return 10;
	}
}

void ChessParser::reset() {
	//white, after black
	//paw, rook, knight, bishop, queen, king
	
	for (int i = 0; i < 16; i++) {
		pieces[i] = std::make_pair( (i%8), (int(i/8)+1)%2);
		pieces[i+16] = std::make_pair( (i%8), int(i/8)+6);
	}


	for (std::size_t i {0}; i < board.size(); i++) {
		for (std::size_t j {0}; j < board[j].size(); j++) {
			board[i][j] = 0;
		}
	}

	for (std::size_t i {0}; i < 8; i++) {
		board[i][1] = 1;
		board[i][6] = 11;
	}

	for (std::size_t i {0}; i < 3; i++) {
		board[i][0] = i+2;
		board[i][7] = 10 + i+2;
		board[7-i][0] = i+2;
		board[7-i][7] = 10 + i+2;
	}

	board[3][0] = 5;
	board[4][0] = 6;

	board[3][7] = 15;
	board[4][7] = 16;

	newX = 0;
	newY = 0;
	oldX = 0;
	oldY = 0;
	eatP = false;
	pieceEating = 0;

	kingMove[0] = true;
	kingMove[1] = true;

	rookMove[0] = true;
	rookMove[1] = true;

	rookMove[2] = true;
	rookMove[3] = true;
}

ChessParser::~ChessParser() {}

std::array<std::array<int, 8>, 8> ChessParser::getArray() const {
	return board;
}


const std::array<std::array<int, 8>, 8> &ChessParser::getArrayPtr() {
	return board;
}

void ChessParser::setArray(std::array<std::array<int, 8>, 8> data) {
	board = data;
}


void ChessParser::draw(const std::array<std::array<int, 8>, 8> &data) {
	std::cout << "\n    ";
	for (char i = 'a'; i <= 'h'; i++) {
		std::cout << i << "   ";
	}

	std::cout << '\n';

	for (int j {7}; j > -1; j--) {

		std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
		std::cout << j+1 << " |";
		for (std::size_t i {0}; i < data.size(); i++) {
			if (data[i][j] == 0) {
				std::cout << "   |";
			} else {
				char c {0};
				switch (data[i][j]%10) {
					case 1:
						c = 'P';
						break;
					case 2:
						c = 'R';
						break;
					case 3:
						c = 'N';
						break;
					case 4:
						c = 'B';
						break;
					case 5:
						c = 'Q';
						break;
					case 6:
						c ='K';
						break;
					default:
						c = 0;
						break;
				}

				if (data[i][j] > 9) {
					c += 32;
				} 
				
				std::cout << ' ' << c << " |";

			}
		}
		std::cout << ' ' << j+1 << '\n';
	}

	std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;


	std::cout << "    ";
	for (char i = 'a'; i <= 'h'; i++) {
		std::cout << i << "   ";
	}

	std::cout << '\n';

	std::cout << std::endl;

}

void ChessParser::draw() {
	draw(board);
}

bool ChessParser::move(int i, int x, int y) {
	oldX = pieces[i].first;
	oldY = pieces[i].second;

	//std::cout << "i is " << i << ' ' << oldX << " " << oldY << std::endl;
	if (i == 18) {
		//std::cout << x << " " << y << " " << old_x << " " << old_y << std::endl;
	}

	if (board[x][y] != 0) {
		for (std::size_t j {0}; j < pieces.size(); j++) {
			if (pieces[j].first == x && pieces[j].second == y) {
				//std::cout << j << " " << pieces[j].first << " " << pieces[j].second << std::endl;
				pieces[j].first = 200;
				pieces[j].second = 200;
				break;
			}
		}
	}

	pieceEating = board[x][y];
	board[x][y] = board[oldX][oldY];
	board[oldX][oldY] = 0;

	pieces[i].first = x;
	pieces[i].second = y;


	return true;
}


int ChessParser::find(char i, int colorPlayer) {
	int result {0};

	switch (i) {
		case 'R':
			result = 8;
			break;
		case 'N':
			result = 9;
			break;
		case 'B':
			result = 10;
			break;
		case 'Q':
			result = 11;
			break;
		case 'K':
			result = 12;
			break;
		default:
			result = 0;
			break;
	}

	return result;

}

std::string ChessParser::chessArraytoFEN(const std::array<std::array<int, 8>, 8> &data) {
	std::string fenStr {""};
	int space {0};

	for (int j {7}; j > -1; j--) {
		for (int i{0}; i < 8; i++) {
			if (data[i][j] == 0) {
				space++;
			} else {
				if (space != 0) {
					fenStr += std::to_string(space);
					space = 0;
				}

				char c {0};
				switch (data[i][j]%10) {
					case 1:
						c = 'P';
						break;
					case 2:
						c = 'R';
						break;
					case 3:
						c = 'N';
						break;
					case 4:
						c = 'B';
						break;
					case 5:
						c = 'Q';
						break;
					case 6:
						c ='K';
						break;
					default:
						c = 0;
						break;
				}

				if (data[i][j] > 9) {
					c += 32;
				}

				fenStr += c;
			}
		}

		if (space != 0) {
			fenStr += std::to_string(space);
			space = 0;
		}

		if (j != 0)
			fenStr += "/";

	}

	return fenStr;
}

std::array<std::array<int, 8>, 8> ChessParser::openStringFEN(const std::string &data) {
	std::array<std::array<int , 8>, 8> chessArray;

	std::size_t x {0};
	std::size_t y {7};

	for (std::size_t i {0}; i < data.size(); i++) {
		//std::cout << x << " " << y << " " << data[i] << std::endl;
		if (data[i] == '/') {
			y--;

			if (y < 0)
				return chessArray;

			x = 0;
		} else if (data[i] == ' ') {
			return chessArray;
		} else {
			if (x > 7)
				continue;

			if (data[i] > 48 && data[i] < 57) {
				//x += (data[i] - 48);
				
				std::size_t j;
				for (j = 0; j < data[i] - 48 && x+j < 8; j++) {
					chessArray[x + j][y] = 0;
				}

				x += j;
			} else {
				int colorP = 0;

				if (data[i] > 96 && data[i] < 122) {
					colorP = 10;
				}

				switch (data[i] - static_cast<int>(colorP*3.2)) {
					case 'R':
						chessArray[x][y] = 2 + colorP;
						x++;
						break;
					case 'N':
						chessArray[x][y] = 3 + colorP;
						x++;
						break;
					case 'B':
						chessArray[x][y] = 4 + colorP;
						x++;
						break;
					case 'Q':
						chessArray[x][y] = 5 + colorP;
						x++;
						break;
					case 'K':
						chessArray[x][y] = 6 + colorP;
						x++;
						break;
					default:
						chessArray[x][y] = 1 + colorP;
						x++;
						break;
				}
			}
		}
	}

	return chessArray;
}

void ChessParser::openStringPgn(const std::string &data) {
	std::string playerAction { "" };
	int colorPlayer = 0;

	//const std::string game { data[0]["game"]["pgn"].get<std::string>() + std::string(" ") };
	//const std::string game { "e4 g5 O-O O-O-O Re1 Re8 Re3 Bd6 Bf4 Nh6 Nc3 Kd8 Kh1 a6 Qd5 " };
	const std::string game { data + std::string(" ") };

	for (std::size_t i {0}; i < game.size(); i++) {
		if (game[i] == ' ') {
			if (playerAction.size() > 0) {
				actionChess(playerAction, colorPlayer);
				colorPlayer = (colorPlayer+1)%2;

				draw();
				getchar();
			}
			playerAction.clear();
		} else {
			//if (game[i] != '+') {
				playerAction += game[i];
			//}
		}
	}
}

void ChessParser::actionChess(std::string playerAction, int colorPlayer) {
	if (playerAction.size() == 2) {
		newX = playerAction[0] - 'a';
		newY = playerAction[1] - '1';

		int result { movePawn(newX, newY, colorPlayer) };

		if (result == -1) {
			std::cout << "error: " << playerAction << std::endl;
		} else {
			move(result, newX, newY);
		}

	} else if (playerAction == "O-O") {
		move(15+colorPlayer*16, 5, colorPlayer*7);
		move(12+colorPlayer*16, 6, colorPlayer*7);

		newX = 9+colorPlayer;
		newY = 1;

		kingMove[colorPlayer] = false;
	} else if (playerAction == "O-O-O") {
		move(8+colorPlayer*16, 3, colorPlayer*7);
		move(12+colorPlayer*16, 2, colorPlayer*7);

		newX = 9+colorPlayer;
		newY = 0;
		kingMove[colorPlayer] = false;
	} else {
		char type { 'P' };

		int position {-1};
		char eat {0};
		eatP = false;

		int end = 2;
		if (playerAction[playerAction.size()-1] == '+') {
			std::cout << "!+!+! Attention Check! !+!+!" << std::endl;

			newX = playerAction[playerAction.size()-3] - 'a';
			newY = playerAction[playerAction.size()-2] - '1';

			end = 3;

			check = true;	
		} else {
			newX = playerAction[playerAction.size()-2] - 'a';
			newY = playerAction[playerAction.size()-1] - '1';

		}

		for (int i {0}; i < playerAction.size()-end; i++) {
			if (i == 0 && playerAction[i] > 64 && playerAction[i] < 90) {
				type = playerAction[i];
			} else if (playerAction[i] == 'x') {
				if (eat != 0) {
					std::cout << "Pb eat " << eat << std::endl;
				}
				eat = 'x';
				eatP = true;
			} else {
				if (position != -1) {
					std::cout << "Pb position " << position << std::endl;
				}

				if (playerAction[i] > 47 && playerAction[i] < 57) {
					position = playerAction[i] - '1' + 10;
				} else if (playerAction[i] > 96 && playerAction[i] < 106) {
					position = playerAction[i] - 'a';
				}
			}
		}

		//std::cout << end << " " << position << " " << playerAction << " "
			//<< eat << " " << eatP << " " << newX << " " << newY << std::endl;


		int result {0};


		switch (playerAction[0]) {
			case 'R':
				result = moveRook(newX, newY, colorPlayer, position, eat);
				break;
			case 'N':
				result = moveKnight(newX, newY, colorPlayer, position, eat);
				break;
			case 'B':
				result = moveBishop(newX, newY, colorPlayer, position, eat);
				break;
			case 'Q':
				result = moveQueen(newX, newY, colorPlayer, position, eat);
				break;
			case 'K':
				result = moveKing(newX, newY, colorPlayer, position, eat);
				break;
			default:
				result = movePawn(newX, newY, colorPlayer, position, eat);
				break;
		}

		if (result == -1) {
			std::cout << "error: " << playerAction << std::endl;
		} else {
			move(result, newX, newY);
		}

	}

}

int ChessParser::movePawn(int x, int y, int colorPlayer, int position, char eat) {
	int ppmove = 0;
	if (colorPlayer == 0) {
		ppmove = 1;
	} else {
		ppmove = -1;
	}
					
	if (eat == 0) {
		for (std::size_t i { static_cast<std::size_t>(colorPlayer*16) }; i < colorPlayer*16 + 8; i++) {
			if (pieces[i].first != 200 && pieces[i].first == x && (pieces[i].second+ppmove == y ||
				(pieces[i].second == 1+colorPlayer*5 && pieces[i].second+ppmove*2 == y))) {
				return i;
			}
		}
	} else {
		for (std::size_t i { static_cast<std::size_t>(colorPlayer*16) }; i < colorPlayer*16 + 8; i++) {
		
			if ( (pieces[i].first != 200 && position == -1 || position == pieces[i].first) &&
			     //((colorPlayer == 0 && (pieces[i].first+1 == x || pieces[i].first-1 == x)) || (colorPlayer == 1 && x-pieces[i].first == 1)) 
			     (pieces[i].first+1 == x || pieces[i].first-1 == x)
			     && std::abs(pieces[i].second - y) == 1) {

				if (board[x][y] == 0) {
					std::cout << "Alert please check your board" << std::endl;

					if (colorPlayer == 0 && y == 5 && board[x][y-1] == 11) {
						for (std::size_t j { 16 }; j < 24; j++) {
							if (x == pieces[j].first && y-1 == pieces[j].second) {
								pieces[j].first = 200;
								pieces[j].second = 200;
								board[x][y-1] = 0;
								break;
							}
						}
					}

					if (colorPlayer == 1 && y == 2 && board[x][y+1] == 1) {
						for (std::size_t j { 0 }; j < 8; j++) {
							if (x == pieces[j].first && y+1 == pieces[j].second) {
								pieces[j].first = 200;
								pieces[j].second = 200;
								board[x][y+1] = 0;
								break;
							}
						}
					}
				}

				return i;
			}
		}
	}

	return -1;
}

int ChessParser::moveQueen(int x, int y, int colorPlayer, int position, char eat) {
	int piece { 11+colorPlayer*16 };

	if (eat == 'x' && board[x][y] == 0) {
		return -1;
	} else if (eat == 0 && board[x][y] != 0) {
		return -1;
	}

	return piece;
}

int ChessParser::moveKing(int x, int y, int colorPlayer, int position, char eat) {
	int piece { 12+colorPlayer*16 };

	if (eat == 'x' && board[x][y] == 0) {
		return -1;
	} else if (eat == 0 && board[x][y] != 0) {
		return -1;
	}

	if (std::abs(pieces[piece].first - x) < 2 && std::abs(pieces[piece].second - y) < 2) {
		kingMove[colorPlayer] = false;
		return piece;
	}


	return -1;
}

int ChessParser::moveKnight(int x, int y, int colorPlayer, int position, char eat) {
	std::array<int, 2> piecesKnight { 9+colorPlayer*16, 14+colorPlayer*16 };

	if (eat == 'x' && board[x][y] == 0) {
		return -1;
	} else if (eat == 0 && board[x][y] != 0) {
		return -1;
	}

	for (const auto &piece : piecesKnight) {
		//std::cout << position << " " << pieces[piece].first << " " << pieces[piece].second << std::endl;
		if ( (position == -1 || position == pieces[piece].first || position-10 == pieces[piece].second) &&
				((std::abs(pieces[piece].first - x) == 1 && std::abs(pieces[piece].second - y) == 2) ||
				(std::abs(pieces[piece].first - x) == 2 && std::abs(pieces[piece].second - y) == 1))
				&& movePosible(piece, x, y, colorPlayer)) {
				return piece;
		}
	}

	return -1;
}

int ChessParser::moveBishop(int x, int y, int colorPlayer, int position, char eat) {
	std::array<int, 2> piecesBishop { 10+colorPlayer*16, 13+colorPlayer*16 };

	if (eat == 'x' && board[x][y] == 0) {
		return -1;
	} else if (eat == 0 && board[x][y] != 0) {
		return -1;
	}

	for (const auto &piece : piecesBishop) {
		if (pieces[piece].first != 200 && std::abs(pieces[piece].first - x) == std::abs(pieces[piece].second - y)
				&& movePosible(piece, x, y, colorPlayer) )
			return piece;
	}

	return -1;
}

int ChessParser::moveRook(int x, int y, int colorPlayer, int position, char eat) {
	std::array<int, 2> piecesRook { 8+colorPlayer*16, 15+colorPlayer*16 };

	if (eat == 'x' && board[x][y] == 0) {
		return -1;
	} else if (eat == 0 && board[x][y] != 0) {
		return -1;
	}

	for (const auto &piece : piecesRook) {
		//std::cout << pieces[piece].first << " " << pieces[piece].second << " " << x << " " << y << " " << position << std::endl;
		if (pieces[piece].first == x && 
		   (position == -1 || (pieces[piece].first == position || pieces[piece].second == position-10))) {

			int ppmove { (y - pieces[piece].second)/std::abs(pieces[piece].second - y)  };

			for (int i = pieces[piece].second+ppmove; i != y+ppmove; i += ppmove) {
				if (i == y && movePosible(piece, x, y, colorPlayer)) {
					rookMove[(piece - colorPlayer*16 - 9)/5 + colorPlayer*2] = false;
					return piece;
				} else if (board[x][i] != 0) {
					break;
				}
			}
		} else if (pieces[piece].second == y && 
		   	   (position == -1 || (pieces[piece].first == position || pieces[piece].second == position-10))) {
			int ppmove { (x - pieces[piece].first)/std::abs(pieces[piece].first - x) };

			for (int i = pieces[piece].first+ppmove; i != x+ppmove; i += ppmove) {
				//std::cout << piece << " " << i << " " << ppmove << " " << x << " " << pieces[piece].first << " " << std::endl;
				if (i == x && movePosible(piece, x, y, colorPlayer)) {
					rookMove[(piece - colorPlayer*16 - 9)/5 + colorPlayer*2] = false;
					return piece;
				} else if (board[i][y] != 0) {
					break;
				}
			}
		}
	}

	return -1;
}

bool ChessParser::movePosible(int i, int x, int y, int colorPlayer) {
	int otherColorPlayer { ((colorPlayer+1) % 2)*10 };
	auto tempArray = board;

	oldX = pieces[i].first;
	oldY = pieces[i].second;

	tempArray[x][y] = tempArray[oldX][oldY];
	tempArray[oldX][oldY] = 0; 
	int king { 6+colorPlayer*10 };
	int kx { pieces[colorPlayer*16 + 12].first };
	int ky { pieces[colorPlayer*16 + 12].second };

	std::array<bool, 8> stopP {true, true, true, true, true, true, true, true};

	for (int i {1}; i < 8; i++) {
		if (stopP[0] && kx - i >= 0 && tempArray[kx - i][ky] != 0) {
			if (tempArray[kx - i][ky] == 2+otherColorPlayer || tempArray[kx - i][ky] == 5+otherColorPlayer) {
				std::cout << "1 " << i << std::endl;
				return false;
			} else {
				stopP[0] = false;
			}
		}

		if (stopP[1] && kx + i < 8 && tempArray[kx + i][ky] != 0) {
			if (tempArray[kx + i][ky] == 2+otherColorPlayer || tempArray[kx + i][ky] == 5+otherColorPlayer) {
				std::cout << "2 " << i << std::endl;
				return false;
			} else {
				stopP[1] = false;
			}
		}
		
		/////////

		if (stopP[2] && kx - i >= 0 && ky - i >= 0 && tempArray[kx - i][ky - i] != 0) {
			if (tempArray[kx - i][ky - i] == 5+otherColorPlayer || 
				 tempArray[kx - i][ky - i] == 4+otherColorPlayer) {
				std::cout << "3 " << i << std::endl;
				return false;
			} else {
				stopP[2] = false;
			}
		}

		if (stopP[3] && kx - i >= 0 && ky + i < 8 && tempArray[kx - i][ky + i] != 0) {
			if (tempArray[kx - i][ky + i] == 5+otherColorPlayer ||
			    tempArray[kx - i][ky + i] == 4+otherColorPlayer) {
				std::cout << "4 " << i << std::endl;
				return false;
			} else {
				std::cout << i << std::endl;
				stopP[3] = false;
			}
		}

		if (stopP[4] && kx + i < 8 && ky - i >= 0 && tempArray[kx + i][ky - i] != 0) {
			if (tempArray[kx + i][ky - i] == 5+otherColorPlayer ||
				tempArray[kx - i][ky - i] == 4+otherColorPlayer) {

				std::cout << "5 " << i << std::endl;
				return false;
			} else {
				stopP[4] = false;
			}
		}

		if (stopP[5] && kx + i < 8 && ky + i < 8 && tempArray[kx + i][ky + i] != 0) {
			if (tempArray[kx + i][ky + i] == 5+otherColorPlayer ||
			    tempArray[kx + i][ky + i] == 4+otherColorPlayer) {
				std::cout << "6 " << i << std::endl;
				return false;
			} else {
				stopP[5] = false;
			}
		}

		////////

		if (stopP[6] && ky - i >= 0 && tempArray[kx][ky - i] != 0) {
			if (tempArray[kx][ky - i] == 2+otherColorPlayer || tempArray[kx][ky - i] == 5+otherColorPlayer) {
				std::cout << "6.1 " << i << std::endl;
				return false;
			} else {
				stopP[6] = false;
			}
		}

		if (stopP[7] && ky + i < 8 && tempArray[kx][ky + i] != 0) {
			if (tempArray[kx][ky + i] == 2+otherColorPlayer || tempArray[kx][ky + i] == 5+otherColorPlayer) {
				std::cout << "6.2 " << i << std::endl;
				return false;
			} else {
				stopP[7] = false;
			}
		}
	}

	if (kx - 2 >= 0) {
		if (ky - 1 >= 0 && tempArray[kx - 2][ky - 1] == 3+otherColorPlayer) {
			std::cout << "7" << std::endl;
			return false;
		}
		
		if (ky + 1 < 8 && tempArray[kx - 2][ky + 1] == 3+otherColorPlayer) {
			std::cout << "8" << std::endl;
			return false;
		}
	}


	if (kx + 2 < 8) {
		if (ky - 1 >= 0 && tempArray[kx + 2][ky - 1] == 3+otherColorPlayer) {
			std::cout << "9" << std::endl;
			return false;
		}
		

		if (ky + 1 < 8 && tempArray[kx + 2][ky + 1] == 3+otherColorPlayer) {
			std::cout << "10" << std::endl;
			return false;
		}
	}

	if (ky - 2 >= 0) {
		if (kx - 1 >= 0 && tempArray[kx - 1][ky - 2] == 3+otherColorPlayer) {
			std::cout << "11" << std::endl;
			return false;
		}

		if (kx + 1 < 8 && tempArray[kx + 1][ky - 2] == 3+otherColorPlayer) {
			std::cout << "12" << std::endl;
			return false;
		}
	}

	if (ky + 2 < 8) {
		if (kx - 1 >= 0 && tempArray[kx - 1][ky + 2] == 3+otherColorPlayer) {
			std::cout << "13" << std::endl;
			return false;
		}

		if (kx + 1 < 8 && tempArray[kx + 1][ky + 2] == 3+otherColorPlayer) {
			std::cout << "14" << std::endl;
			return false;
		}
	}

	return true;
}

void ChessParser::setupPgnAdvance(const std::string line) {
	instructions.clear();
	indexInstruction = 0;

	std::string playerAction {""};

	for (std::size_t i {0}; i < line.size(); i++) {
		if (line[i] == ' ') {
			if (playerAction.size() > 0) {
				instructions.push_back(playerAction);
			}

			playerAction.clear();
		} else {
			//if (line[i] != '+') {
				playerAction += line[i];
			//}
		}
	}

	if (playerAction.size() > 0) {
		instructions.push_back(playerAction);
	}

	reset();
}


void ChessParser::advancePgnStep(ChessMove &cmove) {
	eatP = false;
	check = false;
	actionChess(instructions[indexInstruction], indexInstruction%2);
	indexInstruction++;

	cmove.newX = newX;
	cmove.newY = newY;
	cmove.oldX = oldX;
	cmove.oldY = oldY;
	//cmove.eatP = false;
	cmove.pieceEating = pieceEating;
	cmove.chessArray = board;
	cmove.eatP = eatP;
	cmove.check = check;
}

std::string ChessParser::advanceCurrentFENStep() const {
	std::string fen  { chessArraytoFEN(board) };

	if (indexInstruction%2 == 0)
		fen += " w ";
	else
		fen += " b ";

	for (const auto &p: {0, 1}) {
		if (kingMove[p]) {
			if (rookMove[p*2+1]) {
				//char c = 'K' + p*32;
				fen += static_cast<char>('K' + p*32);
			}
			if (rookMove[p*2]) {
				//char c = 'Q' + p*32;
				//fen += c;
				fen += static_cast<char>('Q' + p*32);
			}
		} else {
			fen += " -";
		}
	}

	fen += " 0 " + std::to_string(indexInstruction+1);

	return fen;
}

std::vector<std::string> &ChessParser::getVector() {
	return instructions;
}
