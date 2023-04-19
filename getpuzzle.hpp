#pragma once
#include <nlohmann/json.hpp>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>

#include <vector>

/// @brief Class to load the puzzle game on lichess
/// @author Thibaud VINCENT
class GetPuzzle {
	public:
		/// @brief Class construter
		GetPuzzle();

		/// @brief Class destructer
		~GetPuzzle();

		/// @brief The puzzle on lichess
		/// @return std::vector
		nlohmann::json getPuzzle();


	private:

};
