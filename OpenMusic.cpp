#include "OpenMusic.hpp"
#include <iostream>

OpenMusic::OpenMusic() {}
OpenMusic::~OpenMusic() {}

void OpenMusic::openMusic(const std::string &filename) {

	/**if (!buffer.loadFromFile(filename)) {
		std::cout << "Error could not open file" << std::endl;
		return;
	}

	const int16_t *samples = buffer.getSamples();
	std::size_t iMax { buffer.getSampleCount() };
	std::size_t rateM { buffer.getSampleRate() };
	std::size_t channels { buffer.getChannelCount() };

	for (std::size_t i {0}; i < iMax; i += channels) {
		data.push_back(samples[i]);
	}

	bpm = std::make_unique<breakfastquay::MiniBPM>(rateM);



	std::size_t offset {0};

	while (data[offset] == 0) {
		offset++;
	}

	offsetTime = static_cast<float>(offset*channels)/static_cast<float>(rateM);
	bpmSpeed = bpm->estimateTempoOfSamples(data.data(), data.size());

	std::cout << "BPM: " << bpmSpeed << std::endl;
	std::cout << "Offset: " << offsetTime << std::endl;**/


	unsigned int samplerate = 0;
	unsigned int win_s = 1024;
	unsigned int hop_size = win_s / 4;
	double start = 0.0;
	double duration = 60.0;

	aubio_source_t* s = new_aubio_source(filename.c_str(), samplerate, hop_size);

	size_t ui;

	if (!s) {
		std::cout << "Source is not valid" << std::endl;
		return;
	}

	Samples music = Samples(s, hop_size);
	MarkProgress(0, "Read audio samples");

	TempoDetector tempo = TempoDetector(music, start, 
			std::min(duration, (double)aubio_source_get_duration(s) / samplerate));
	tempo.exec();
	
	results = tempo.getResult();

	for (TempoResult result : results) {
		PrintOut("[RESULT] " + std::to_string(result.bpm) + " BPM, offset @ " + std::to_string(result.offset) + " sec, fitness " + std::to_string(result.fitness));

	}

	del_aubio_source(s);
	aubio_cleanup();
}


const std::vector<std::size_t> &OpenMusic::getAmplitude() {
	return amplitudeMax;
}

const std::vector<std::size_t> &OpenMusic::getTime() {
	return indexMax;
}

const std::size_t OpenMusic::getRate() const {
	return buffer.getSampleRate();
}

const std::size_t OpenMusic::getTimeMusic() const {
	return buffer.getSampleCount()/buffer.getChannelCount();
}


const double OpenMusic::getBPM() {
	if (results.size() > 0) {
		return results[0].bpm;
	}

	return 0;
}

const double OpenMusic::getOffset() {
	if (results.size() > 0) {
		return results[0].offset;
	}

	return 0;
}
