#pragma once

#include "ChessParser.hpp"
#include "default.hpp"

/// @brief the goal of this class is to find trigger event
/// @author Thibaud VINCENT
class DetectChess : public ChessParser {
	public:
		DetectChess();
		~DetectChess();

		/// @brief the goal of this function is to find trigger event
		/// @return std::vector<std::string>
		std::vector<std::string> triggerEvents();

		/// @brief get the attack of the piece
		/// @param piece the pieces
		/// @param colorPlayer the color of the player
		/// @return std::vector<int>
		void doubleAttack(std::vector<std::string> trigE, int colorPlayer);
};
