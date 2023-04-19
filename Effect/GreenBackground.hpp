#pragma once

#include "effect.hpp"
#include <string>
#include "../default.hpp"
#include "../camera.hpp"

namespace Effect {

class GreenBackground : public Main {
	public:
		GreenBackground(std::string videoName, float imgSizeX, float imgSizeY, float imgPosX, float imgPosY,
				float timeStart, float timeEnd, float timeVideo);

		~GreenBackground();


		/// @brief to set the effct for the video
		/// @param effect the image who will get the effect (CV_8UC3)
		virtual void setEffect(cv::Mat &effect, float fps, float numberFPS, const ChessMove &cmove, RectEffect &rect);

		/// @brief replace green background to image
		/// @return void
		void setImageForground(const cv::Mat &effect, cv::Mat &chess, std::size_t startX, std::size_t startY);

		/// @brief return the number of frame
		virtual float getTimeEffect(float defaultFrameRate) const;

	protected:
		std::size_t m_imgSizeX;
		std::size_t m_imgSizeY;

		float m_imgPosX;
		float m_imgPosY;

		float framerate;
		float nbrFrame;

		float height;
		float width;

		std::string m_file;

		cv::VideoCapture m_video;
		cv::Mat m_currentVideoImg;
		std::size_t m_indexImg;
		//std::vector<cv::Mat> m_imgV;

		int startX;
		int startY;

		const float m_timeStart;
		float m_timeEnd;
		const float m_timeVideo;
		float nbrFrameTime;
};

} // namespace Effect
