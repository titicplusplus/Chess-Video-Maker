#include "SaveMusic.hpp"
#include <iostream>

SaveMusic::SaveMusic() {}
SaveMusic::~SaveMusic() {}

std::size_t SaveMusic::getMusicTime(const std::string name, const PlaySound &audio) {
	if (audio.timeEnd == -1)
		return (audioBuffer[name].getSampleCount()/audioBuffer[name].getChannelCount());
		
	return (audio.timeEnd - audio.timeStart)*MRATE;///((audioBuffer[name].getSampleCount()/audioBuffer[name].getChannelCount())/MRATE)*MRATE;
}

bool SaveMusic::createAudioFile(std::vector<PlaySound> musicTimer, std::size_t videoTime) {
	if (musicTimer.empty())
		return false;

	audioBuffer.clear();

	std::vector<std::size_t> endC;

	float maxValue {0};
	std::size_t max {0};

	for (const auto& music : musicTimer) {
		if (!audioBuffer.contains(music.filename)) {
			audioBuffer[music.filename] = sf::SoundBuffer();

			if (!audioBuffer[music.filename].loadFromFile(music.filename)) {
				std::cout << "Error loading " << music.filename << std::endl;
				return false;
			}
		}

		std::size_t start = music.startT;

		for (auto it { endC.begin() }; it != endC.end();) {
			if (*it < start) {
				endC.erase(it);
			} else {
				it++;
			}
		}

                endC.push_back(music.startT + getMusicTime(music.filename, music));
		
		if (endC.size() > maxValue)
			maxValue = endC.size();
	}

	if (maxValue == 0) {
		std::cout << "numvalue equal to zero" << std::endl;
		return false;
	}

	audio.clear();

	std::size_t indexMT {0};

	std::vector<std::reference_wrapper<PlaySound>> elemAudio;
	for (std::size_t i {0}; i < videoTime; i++) {
		while (indexMT < musicTimer.size() && musicTimer[indexMT].startT == i) {
			elemAudio.push_back( musicTimer[indexMT] );
			musicTimer[indexMT].compteur = musicTimer[indexMT].timeStart*MRATE*audioBuffer[musicTimer[indexMT].filename].getChannelCount();
			indexMT++;
		}

		int16_t result {0};

		for (auto it { elemAudio.begin() }; it != elemAudio.end();) {
			if (it->get().timeEnd == -1) {
				if (audioBuffer[it->get().filename].getSampleCount() <= it->get().compteur) {
					elemAudio.erase(it);
				} else {
					result += (static_cast<float>
						(audioBuffer[it->get().filename].getSamples()[it->get().compteur])*it->get().volume)
						/maxValue;

					it->get().compteur += audioBuffer[it->get().filename].getChannelCount();
					it++;
				}
			} else {
				if (audioBuffer[it->get().filename].getChannelCount()*MRATE*it->get().timeEnd <= it->get().compteur) {
					elemAudio.erase(it);
					continue;
				} else {
					result += (static_cast<float>
						(audioBuffer[it->get().filename].getSamples()[it->get().compteur])*it->get().volume)
						/maxValue;

					it->get().compteur += audioBuffer[it->get().filename].getChannelCount();
					it++;
				}
			}


			/**if (audioBuffer[it->get().filename].getSampleCount() <= it->get().compteur) {
				elemAudio.erase(it);
			} else {
				result += (static_cast<float>
					(audioBuffer[it->get().filename].getSamples()[it->get().compteur])*it->get().volume)
					/maxValue;

				it->get().compteur += audioBuffer[it->get().filename].getChannelCount();


				it++;
			}**/
		}

		audio.push_back(result);
	}

	return true;
}

void SaveMusic::saveToFile(const std::string& filename) {
	sf::SoundBuffer buffer;

	std::cout << "The time is " << static_cast<double>(audio.size())/MRATE << "\n";

	buffer.loadFromSamples(&audio[0], audio.size(), 1, MRATE);
	buffer.saveToFile(filename);
}
