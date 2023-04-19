#include "voice.hpp"
#include <curl/curl.h>
#include <SFML/Audio.hpp>

#include <fstream>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif

namespace Effect {

size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *s)
{
	s->append(static_cast<char *>(ptr), size*nmemb);
	return size*nmemb;
}

Voice::Voice(const std::string text, int colorPlayer) : m_text {text}, filenameWAV { "Ressources/voices/" + text + ".wav" }  {
	this->m_c = colorPlayer;
	getSizeText();

	if (!fs::exists(filenameWAV)) {
		openRessource = std::async([this] {
			std::cout << "Go open the file" << '\n';
			const std::string filenameMP3 { "Ressources/voices/" + m_text + ".mp3" };
			
			CURL *curl = curl_easy_init();

			if (curl) {
				std::string s {""};

				curl_easy_setopt(curl, CURLOPT_URL, std::string(
"https://zhiwei-tech.com/cgi-bin/ekho2.pl?cmd=SAVEMP3&voice=BaiduMandarinMale&speedDelta=10&pitchDelta=0&volumeDelta=0&text="
				+ m_text).c_str());

				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);


				CURLcode res = curl_easy_perform(curl);

				if (res == CURLE_OK) {
					std::ofstream file { filenameMP3 };
					file << s;
					file.close();

					std::cout << "Go\n";


					system(("ffmpeg -y -i \""  + filenameMP3 + "\" -acodec pcm_s16le -ar 44100 -ac 1 \"" +
								filenameWAV + "\"").c_str());
		
				} else {
					std::cerr << "Error with curl\n";
				}

				curl_easy_cleanup(curl);
			}

			setTimeEffect();
			std::cout << "End" << "\n";
		});

	} else {
		openRessource = std::async(std::bind(&Voice::setTimeEffect, this));
	}


}

void Voice::setTimeEffect() {
	sf::SoundBuffer buffer;

	if (!buffer.loadFromFile(filenameWAV)) {
		std::cerr << "Can't load file: " << filenameWAV << std::endl;
		return;
	}

	m_timeEffect = static_cast<float>(buffer.getSampleCount())/static_cast<float>(buffer.getSampleRate());

}

void Voice::getSizeText() {
	int baseline = 0;
	cv::Size s = cv::getTextSize(m_text, cv::FONT_HERSHEY_SIMPLEX, 2, 2, &baseline);

	cv::Mat img = cv::Mat(cv::Size(s.width*1.8, s.height + baseline), CV_8UC3, cv::Scalar(255, 255, 255));
	cv::putText(img, m_text, cv::Point(0, s.height), cv::FONT_HERSHEY_TRIPLEX, 2, cv::Scalar(0, 0, 0), 4);

	//cv::imshow("ui", img);
	//cv::waitKey(0);

	for (std::size_t ix = img.cols-1; ix != 0; ix--) {
		for (std::size_t posy {0}; posy < img.rows; posy++) {
			if (img.at<cv::Vec3b>(posy, ix)[0] != 255) {
				m_textSize = ix/2;
				return;
			}
		}
	}

	std::cout << "ERROR can't count text size" << std::endl;

	m_textSize = 0;
	return;
}


void Voice::setEffect(cv::Mat &effect, float fps, float numberFPS, const ChessMove &cmove, RectEffect &rect) {
	openRessource.wait();
	cv::putText(effect, m_text, cv::Point(effect.cols/2 - m_textSize, effect.rows*0.8),
			cv::FONT_HERSHEY_TRIPLEX, 2, cv::Scalar(0, 0, 0), 12);

	cv::putText(effect, m_text, cv::Point(effect.cols/2 - m_textSize, effect.rows*0.8),
			cv::FONT_HERSHEY_TRIPLEX, 2, cv::Scalar(7, 238, 222), 4);
}

float Voice::getTimeEffect(float defaultFrameRate) const {
	openRessource.wait();
	return m_timeEffect + 0.1;
}

Voice::~Voice() {
	openRessource.wait();
}

} // namespace Effect
