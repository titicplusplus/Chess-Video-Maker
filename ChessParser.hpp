#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <array>

#include <utility>
#include "default.hpp"

//typedef std::array<std::array<int, 8>, 8> ChessArray;



/// @brief Class to read and execute Pgn std::string
/// @author Thibaud VINCENT
class ChessParser {
	public:
		/**enum {
			EMPTY = 0,
			BLACK_PAWN = 11,
			BLACK_ROOK = 12,
			BLACK_KNIGHT = 13,
			BLACK_BISHOP = 14,
			BLACK_QUEEN = 15,
			BLACK_KING = 16,

			WHITE_PAWN = 1,
			WHITE_ROOK = 2,
			WHITE_KNIGHT = 3,
			WHITE_BISHOP = 4,
			WHITE_QUEEN = 5,
			WHITE_KING = 6,
		};**/


		ChessParser();
		~ChessParser();

		/// @brief to reset all of std::array
		void reset();

		/// @brief to render of std::array for Human
		void draw();


		/// @brief to render of std::array for Human with std::array
		static void draw(const std::array<std::array<int, 8>, 8> &data);

		/// @brief get the number of point
		/// @param piece the piece
		/// @return int
		static int pointOfPiece(int piece);

		/// @brief return the std::array, use openStringPgn or openStringFEN before
		/// @return std::array<std::array<int, 8>, 8> (x/y position)
		std::array<std::array<int, 8>, 8> getArray() const;

		/// @brief return the std::array, use openStringPgn or openStringFEN before
		/// @return const std::array<std::array<int, 8>, 8>& (x/y position)
		const std::array<std::array<int, 8>, 8> &getArrayPtr();

		/// @brief return the std::array, use openStringPgn or openStringFEN before
		/// @return std::array<std::array<int, 8>, 8> (x/y position)
		void setArray(std::array<std::array<int, 8>, 8> data);

		/// @brief move piece, return if true if it's word
		/// @param i pieces index
		/// @param x new x coord
		/// @param y new y
		/// @return bool
		bool move(int i, int x, int y);

		/// @brief find piece
		/// @param i is the piece
		/// @param colorPlayer the player (white or black)
		/// @return int
		int find(char i, int colorPlayer);

		/// @brief Open the Pgn data to convert it into std::array
		/// @param data std::string with the Pgn data
		void openStringPgn(const std::string &data);

		/// @brief Open the FEN data to convert it into std::array
		/// @param data std::string with the FEN data
		static std::array<std::array<int, 8>, 8> openStringFEN(const std::string &data);

		/// @brief Open the std::array of data to convert it into std::string (FEN)
		/// @param data std::string with the FEN data
		static std::string chessArraytoFEN(const std::array<std::array<int, 8>, 8> &data);

		/// @brief Advance the game of a movement
		/// @param playerAction the action
		/// @param colorPlayer the current player
		void actionChess(std::string playerAction, int colorPlayer);

		/// @brief Move Rook
		/// @param x new coord
		/// @param y new coord
		/// @param colorPlayer the current player
		/// @param double position possible
		/// @param eat piece of chess
		/// @return int 
		int moveRook(int x, int y, int colorPlayer, int position = -1, char eat = 0);

		/// @brief Move Knigth 
		/// @param x new coord
		/// @param y new coord
		/// @param colorPlayer the current player
		/// @param double position possible
		/// @param eat piece of chess
		/// @return int
		int moveKnight(int x, int y, int colorPlayer, int position = -1, char eat = 0);

		/// @brief Move Bishop
		/// @param x new coord
		/// @param y new coord
		/// @param colorPlayer the current player
		/// @param double position possible
		/// @param eat piece of chess
		/// @return int
		int moveBishop(int x, int y, int colorPlayer, int position = -1, char eat = 0);

		/// @brief Move Queen
		/// @param x new coord
		/// @param y new coord
		/// @param colorPlayer the current player
		/// @param double position possible
		/// @param eat piece of chess
		/// @return int
		int moveQueen(int x, int y, int colorPlayer, int position = -1, char eat = 0);

		/// @brief Move King
		/// @param x new coord
		/// @param y new coord
		/// @param colorPlayer the current player
		/// @param double position possible
		/// @param eat piece of chess
		/// @return int 
		int moveKing(int x, int y, int colorPlayer, int position = -1, char eat = 0);

		/// @brief Move Pawn
		/// @param x new coord
		/// @param y new coord
		/// @param colorPlayer the current player
		/// @param double position possible
		/// @param eat piece of chess
		/// @return int 
		int movePawn(int x, int y, int colorPlayer, int position = -1, char eat = 0);


		/// @brief 
		/// @param i new coord
		/// @param x new coord
		/// @param y new coord
		/// @param colorPlayer the current player
		/// @return bool true if possible
		bool movePosible(int i, int x, int y, int colorPlayer);


		/// @brief setup parser to extract pgn step after step
		/// @param line the line instruction
		/// @return void
		void setupPgnAdvance(const std::string line);

		/// @brief advance with one step
		/// @return void
		void advancePgnStep(ChessMove &cmove);

		/// @brief get the FEN of the current step of 'advance'
		/// @warning you must use it with the 'advance' part
		/// @return std::string
		std::string advanceCurrentFENStep() const;

		/// @brief get vector<std::string> of instruction
		/// @return &std::vector<std::string> 
		std::vector<std::string> &getVector();

	protected:
		std::array<std::pair<int, int>, 32> pieces;
		std::array<std::array<int, 8>, 8> board;

		std::size_t indexInstruction;
		std::vector<std::string> instructions;

		std::array<bool, 2> kingMove;
		std::array<bool, 4> rookMove;

		int newX;
		int newY;
		int oldX;
		int oldY;
		bool eatP;
		bool check;
		int lastP;
		int pieceEating;
};
