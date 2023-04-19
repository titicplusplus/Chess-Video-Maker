#include "GreenBackground.hpp"

namespace Effect {

GreenBackground::GreenBackground(std::string videoName, float imgSizeX, float imgSizeY, float imgPosX, float imgPosY,
				float timeStart, float timeEnd, float timeVideo) :
				m_timeStart {timeStart}, m_timeEnd {timeEnd}, m_timeVideo{timeVideo}{

	this->m_imgPosX = imgPosX;
	this->m_imgPosY = imgPosY;
	m_file = videoName;

	openRessource = std::async([this, imgSizeX, imgSizeY] {
		if (!m_video.open("Ressources/video/" + m_file)) {
			std::cerr << "Warning can't open video: " << m_file << std::endl;
			return;
		}

		framerate = m_video.get(cv::CAP_PROP_FPS);
		nbrFrame = m_video.get(cv::CAP_PROP_FRAME_COUNT);

		height = m_video.get(cv::CAP_PROP_FRAME_HEIGHT);
		width = m_video.get(cv::CAP_PROP_FRAME_WIDTH);

		float currentFrame {0};
		float add {1/framerate};

		if (m_timeEnd == -1) {
			m_timeEnd = nbrFrame/framerate;
		}


		while (1) {

			if (currentFrame >= m_timeStart) {
				break;
			}

			cv::Mat imgV;
			m_video >> imgV;

			currentFrame  += add;

			if (imgV.empty()) {
				break;
			}
		}

		//m_video.release();

		if (imgSizeX == -1 && imgSizeY == -1) {
			this->m_imgSizeX = VIDEO_X - 1;
			this->m_imgSizeY = VIDEO_Y - 1;
			startX = 0;
			startY = 0;
		} else {
			
			if (imgSizeX == -1) {
				this->m_imgSizeX = VIDEO_X - 2;
				this->m_imgSizeY = m_imgSizeX*height/width;
			} else if (imgSizeY == -1) {
				this->m_imgSizeY = VIDEO_Y - 2;
				this->m_imgSizeX = m_imgSizeY*width/height;
			} else {
				this->m_imgSizeX = width*imgSizeX;
				this->m_imgSizeY = height*imgSizeY;
			}

			startX = m_imgPosX*VIDEO_X - m_imgSizeX/2;
			startY = m_imgPosY*VIDEO_Y - m_imgSizeY/2;

			if (startX < 0) {
				startX = 0;
			}

			if (startY < 0) {
				startY = 0;
			}

			if (startX + m_imgSizeX >= VIDEO_X) {
				startX = VIDEO_X - 1;
			}

			if (startY + m_imgSizeY >= VIDEO_Y) {
				startY = VIDEO_Y - 1;
			}
		}


		//nbrFrame = m_imgV.size();
		//nbrFrameTime = (m_timeEnd - m_timeStart)*nbrFrame;

		m_indexImg = 0;
		cv::Mat tmpImage;
		m_video >> tmpImage;

		if (tmpImage.empty()) {
			std::cerr << "Error\n";
		}

		cv::resize(tmpImage, m_currentVideoImg, cv::Size(m_imgSizeX, m_imgSizeY));


	});
}

void GreenBackground::setEffect(cv::Mat &effect, float fps, float numberFPS, const ChessMove &cmove, RectEffect &rect) {
	const float startFPS { m_timeVideo };

	if (fps > startFPS) {
		openRessource.wait();

		std::size_t index { ((fps - startFPS)*nbrFrame/(numberFPS - startFPS))*framerate/60.0  };
			
			//m_timeStart*nbrFrame + (fps - startFPS)/(numberFPS - startFPS)*nbrFrameTime };
		if (index >= nbrFrame) {
			std::cerr << "weird Index: " << index << " " << nbrFrame << " " << m_file << '\n';
			index = nbrFrame - 1;

			if (nbrFrame == -1) {
				return;
			}
		}

		if (index != m_indexImg) {
			m_indexImg = index;

			
			cv::Mat tmpImage;
			m_video >> tmpImage;

			cv::resize(tmpImage, m_currentVideoImg, cv::Size(m_imgSizeX, m_imgSizeY));
		}

		//cv::Mat fimg;
		//cv::resize(m_imgV[index], fimg, cv::Size(m_imgSizeX, m_imgSizeY));
		setImageForground(m_currentVideoImg, effect, startX, startY);
	}
}

void GreenBackground::setImageForground(const cv::Mat &effect, cv::Mat &chess, std::size_t startX, std::size_t startY) {
	for (std::size_t x {0}; x < effect.cols && x+startX < chess.cols; x++) {
		for (std::size_t y {0}; y < effect.rows && y+startY < chess.rows; y++) {
			cv::Vec3b pixelE { effect.at<cv::Vec3b>(cv::Point(x, y)) };

			int diff = 0;

			if (pixelE[0] > pixelE[2]) {
				diff = pixelE[0] - pixelE[2];
			} else {
				diff = pixelE[2] - pixelE[1];
			}

			if (pixelE[1] > 20 && pixelE[0] + pixelE[2] < static_cast<float>(pixelE[1])*1.4) {
				continue;
			} else {
				cv::Point cp = cv::Point(startX + x, startY + y);
				chess.at<cv::Vec3b>(cp)[0] = pixelE[0];
				chess.at<cv::Vec3b>(cp)[1] = pixelE[1];
				chess.at<cv::Vec3b>(cp)[2] = pixelE[2];
			}
		}
	}
}

float GreenBackground::getTimeEffect(float defaultFrameRate) const {
	openRessource.wait();

	if (nbrFrame == -1) {
		std::cout << "Error can't read " << m_file << "\n";
		return 0;
	}

	return (m_timeEnd - m_timeStart) + m_timeVideo/defaultFrameRate;
}

GreenBackground::~GreenBackground() {
	m_video.release();
}

} // namespace Effect
