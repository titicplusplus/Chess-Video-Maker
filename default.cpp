#include "default.hpp"
#include <chrono>

void getType(const cv::Mat &ui) {
	std::string r;

	uchar depth = ui.type() & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (ui.type() >> CV_CN_SHIFT);

	switch ( depth ) {
		case CV_8U:  r = "8U"; break;
		case CV_8S:  r = "8S"; break;
		case CV_16U: r = "16U"; break;
		case CV_16S: r = "16S"; break;
		case CV_32S: r = "32S"; break;
		case CV_32F: r = "32F"; break;
		case CV_64F: r = "64F"; break;
		default:     r = "User"; break;
	}

	r += "C";
	r += (chans+'0');

	std::cout << r << std::endl;
}



std::size_t ChessMove::getCenterX(bool bar) const {
	if (newX > 8) {
		return bar*CASE_BAR + newY*CASE_ALL;
	}

	return (newX+0.5)*CASE_SIZE + CASE_BAR*bar;
}

std::size_t ChessMove::getCenterY() const {
	if (newX > 8) {
		return (1 - (newX - 9))*CASE_ALL;
	}

	return (7-newY + 0.5)*CASE_SIZE;
}

PlaySound::PlaySound(std::string filename, std::size_t startT, float timeStart, float timeEnd, float volume) {
	this->filename = filename;
	this->startT = startT;
	this->timeStart = timeStart;
	this->timeEnd = timeEnd;
	this->volume = volume;	

	this->compteur = 0;
}

PlaySound::~PlaySound() {}

void PlaySound::draw() const {
	std::cout << filename << " " << startT << " " << volume << std::endl;
}

namespace Default {

void removeBlack(cv::Mat black) {
	cv::Size s { black.size() };

	for (std::size_t startX {0}; startX < s.width; startX++) {
		for (std::size_t startY {0}; startY < s.height; startY++) {
			cv::Vec4f pixel = black.at<cv::Vec4b>(startY, startX);

			if (pixel[0] <= 5 && pixel[1] <= 5 && pixel[2] <= 5) {
				//pixel[4] = 0;
				black.at<cv::Vec4b>(startY, startX)[3] = 0;
			}
		}
	}
}

void alphaComposing(cv::Mat& back, const cv::Mat& front, std::size_t posX, std::size_t posY) {
	cv::Size bsize { back.size() };
	cv::Size fsize { front.size() };

	//std::cout << bsize  << " " << fsize << " " << posX << " " << posY << "\n";

	for (std::size_t startX {posX}; startX < posX + fsize.width && startX < bsize.width; startX++) {
		for (std::size_t startY {posY}; startY < posY + fsize.height && startY < bsize.height; startY++) {
			cv::Vec4f bpixel { back.at<cv::Vec4b>(startY, startX) };
			cv::Vec4f fpixel { front.at<cv::Vec4b>(startY - posY, startX - posX) };

			fpixel[3] /= 255;
			bpixel[3] /= 255;

			for (std::size_t i {0}; i < 3; i++) {
				back.at<cv::Vec4b>(startY, startX)[i] 
					= (fpixel[i]*fpixel[3] + bpixel[3]*bpixel[i]*(1 - fpixel[3]));
			}

			back.at<cv::Vec4b>(startY, startX)[3] 
					= ((1 - (1 - fpixel[3]) * (1 - bpixel[3])) * 255);
		}
	}
}

cv::Mat setTransparency(const cv::Mat &piece, float alpha) {
	cv::Size psize { piece.size() };

	cv::Mat pieceAlpha = cv::Mat(psize, CV_8UC4);

	for (std::size_t posX {0}; posX < psize.width; posX++) {
		for (std::size_t posY {0}; posY < psize.height; posY++) {
			cv::Vec4f ppixel { piece.at<cv::Vec4b>(posY, posX) };

			pieceAlpha.at<cv::Vec4b>(posY, posX)[0] = ppixel[0];
			pieceAlpha.at<cv::Vec4b>(posY, posX)[1] = ppixel[1];
			pieceAlpha.at<cv::Vec4b>(posY, posX)[2] = ppixel[2];
			pieceAlpha.at<cv::Vec4b>(posY, posX)[3] = ppixel[3]*alpha;
		}
	}

	return pieceAlpha;
}

unsigned long randomTime(unsigned long start, unsigned long end) {
	return std::chrono::duration_cast<std::chrono::microseconds>(
 		std::chrono::system_clock::now().time_since_epoch()
	).count()%(end - start) + start;
}

std::pair<int, int> findPieces(const int pieces, const std::array<std::array<int, 8>, 8> &chessArray) {
	std::pair<int, int> pos {std::make_pair(0, 0)};
	for (; pos.first < chessArray.size(); pos.first++) {
		for (pos.second = 0; pos.second < chessArray[pos.first].size(); pos.second++) {
			if (chessArray[pos.first][pos.second] == pieces) {
				return pos;
			}
		}
	}

	return std::make_pair(-1, -1);
}


} // namespace Default
