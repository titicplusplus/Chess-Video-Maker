#pragma once

#include "BPMLib/MiniBpm.h"
#include <string>
#include <SFML/Audio.hpp>
#include "BPMLib/FindTempo_standalone.hpp"
#include <vector>

/// @brief OpenMusic to get the db
/// @author Thibaud VINCENT
class OpenMusic {
	public:
		OpenMusic();
		~OpenMusic();

		/// @brief Open a music and find the db of the signal
		/// @param filename the file name
		/// @return void
		void openMusic(const std::string &filename);

		/// @brief get the reference of the amplitude max
		/// @return std::vector<int16_t>&
		const std::vector<std::size_t> &getAmplitude();

		/// @brief get the reference of the index in the music max
		/// @return std::vector<int16_t>&
		const std::vector<std::size_t> &getTime();

		/// @brief the rate of the music
		/// @return const std::size_t
		const std::size_t getRate() const;

		/// @brief the rate of the music
		/// @return number of value std::size_t
		const std::size_t getTimeMusic() const;

		/// @brief get the bpm after call openMusic
		/// @return double
		const double getBPM();


		/// @brief get the offset time after call openMusic
		/// @return double
		const double getOffset();
	private:
		std::vector<std::size_t> amplitudeMax;
		std::vector<std::size_t> indexMax;
		std::vector<float> data;

		sf::SoundBuffer buffer;

		std::unique_ptr<breakfastquay::MiniBPM> bpm;

		//double bpmSpeed;
		//double offsetTime;

		TempoResults results;
};
