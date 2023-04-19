#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include "default.hpp"

/// @brief the goal of this class is, with an std::vector, create the audio file for the video
/// @author Thibaud VINCENT
class SaveMusic {
	public:
		/// @brief Class constructor
		SaveMusic();

		/// @brief Class destructor
		~SaveMusic();

		/// @brief open the std::vector<std::pair<std::size_t, std::string>> to render an std::vector<int16_t>
		/// @param musicTimer the std::vector<std::pair(std::size_t, "musique")> 
		/// @param videoTime the number of frame
		/// @return bool
		bool createAudioFile( std::vector<PlaySound> musicTimer, std::size_t videoTime );

		/// @brief get music time (with rate)
		/// @param name the music name
		/// @return std::size_t
		std::size_t getMusicTime(const std::string name, const PlaySound &audio);

		/// @brief save to file
		/// @param filename the name of the file
		/// @return void
		void saveToFile(const std::string& filename);
	private:
		std::vector<int16_t> audio;
		std::unordered_map<std::string, sf::SoundBuffer> audioBuffer;
};
