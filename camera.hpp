#pragma once
#include <opencv2/opencv.hpp>
#include "default.hpp"

//#include "chessparser.hpp"


/// @brief the goal of this class is to move the camera
class Camera {
	public:
		Camera();
		~Camera();

		/// @brief to move camera between piece
		/// @param chess the image of th current position
		/// @param video the chess video
		/// @param fpsTime the fps max
		void moveCam(const cv::Mat &chess, cv::VideoWriter &video, int fpsTime, std::size_t centerX, std::size_t centerY);

		/// @brief to zoom on the piece who move
		/// @param chess the current image
		void zoomCenterPoint(cv::Mat &chess, std::size_t centerX, std::size_t centerY);
		void zoomPosPoint(cv::Mat &chess, std::size_t posX, std::size_t posY);

		void centerToPoint(const cv::Mat &chess, std::size_t &posX, std::size_t &posY,
				const std::size_t &centerX, const std::size_t &centerY);
	
		void setDefaultCenter(const cv::Mat &chess, const std::size_t &centerX, const std::size_t &centerY);

		void setRotate(cv::Mat &chess, const std::size_t &centerX, const std::size_t &centerY, float angle);

		void setRatio(float cratio);

		std::size_t getSizeX() const;
		std::size_t getSizeY() const;

		/// @brief set a new rect for the camera
		/// @param newRect the new rect
		/// @return void
		void setRect(const RectEffect &newRect);

	private:
		RectEffect rect;
		float ratioF;
};

