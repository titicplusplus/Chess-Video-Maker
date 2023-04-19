#include "camera.hpp"

Camera::Camera() {
	rect.x = 0;
	rect.y = 0;

	ratioF = 0.6;

	rect.height = VIDEO_Y*ratioF;
	rect.width = VIDEO_X*ratioF;
}

Camera::~Camera() {}


void Camera::moveCam(const cv::Mat &chess, cv::VideoWriter &video, int fpsTime, std::size_t centerX, std::size_t centerY) {
	if (rect.height == 0) {
		rect.height = chess.rows * ratioF;
		rect.width = chess.cols * ratioF;
		return;
	}

	std::size_t posX, posY;

	centerToPoint(chess, posX, posY, centerX, centerY);

	float moveX { (static_cast<float>(posX) - static_cast<float>(rect.x))/static_cast<float>(fpsTime)  };
	float moveY { (static_cast<float>(posY) - static_cast<float>(rect.y))/static_cast<float>(fpsTime)  };

	for (float i {0}; i < fpsTime; i++) {
		cv::Mat currentV = chess.clone();
		//std::cout << moveX << " " << moveY << " " << i << " " << rect.x << " " << rect.y << std::endl;

		zoomPosPoint(currentV, rect.x + moveX*i, rect.y + moveY*i);
		video.write(currentV);
	}
}

void Camera::zoomPosPoint(cv::Mat &chess, std::size_t posX, std::size_t posY) {
	/**std::cout << posX << " " << posY << " " << rect.width << " " << rect.height << " "
		<< chess.rows << " " << chess.cols << " "
		<< posX+rect.width << " " << posY+rect.height << std::endl;

	if (posX == 711 && posY == 785) {
		cv::imshow("ui", chess);
		cv::waitKey(0);


		cv::imshow("ui", chess(cv::Rect(posX, posY, rect.width, rect.height)));
		cv::waitKey(0);
	}**/

	cv::Mat selected { chess(cv::Rect(posX, posY, rect.width, rect.height)).clone() };

	cv::resize(selected, chess, cv::Size(chess.cols, chess.rows), 1, 1, cv::INTER_LINEAR);
}


void Camera::zoomCenterPoint(cv::Mat &chess, std::size_t centerX, std::size_t centerY) {
	std::size_t posX, posY;
	centerToPoint(chess, posX, posY, centerX, centerY);
	zoomPosPoint(chess, posX, posY);
}


void Camera::centerToPoint(const cv::Mat &chess, std::size_t &posX, std::size_t &posY,
		const std::size_t &centerX, const std::size_t &centerY) {

	if (centerX >= rect.width/2)
		posX = centerX - rect.width/2;
	else
		posX = 0;
	
	if (centerY >= rect.height/2)
		posY = centerY - rect.height/2;
	else
		posY = 0;

	///std::cout << posY + rect.heigth << " " << chess.rows << std::endl;
	if (posX+rect.width >= chess.cols) {
		posX = chess.cols - rect.width - 1;
	}

	if (posY+rect.height >= chess.rows) {
		posY = chess.rows - rect.height - 1;
	}
}


void Camera::setDefaultCenter(const cv::Mat &chess, const std::size_t &centerX, const std::size_t &centerY) {
	std::size_t posX, posY;
	centerToPoint(chess, posX, posY, centerX, centerY);
	rect.x = posX;
	rect.y = posY;
}

void Camera::setRotate(cv::Mat &chess, const std::size_t &centerX, const std::size_t &centerY, float angle) {
	std::size_t posX, posY;
	centerToPoint(chess, posX, posY, centerX, centerY);	

	cv::Mat ui = chess.clone();
	cv::Point2f pc(posX + rect.width/2, posY + rect.height/2);
	
	float scale {};

    	cv::Mat r = cv::getRotationMatrix2D(pc, angle, 1/ratioF);

	cv::warpAffine(ui, chess, r, chess.size()); // what size I should use?

}

std::size_t Camera::getSizeX() const {
	return rect.width;
}

std::size_t Camera::getSizeY() const {
	return rect.height;
}

void Camera::setRatio(float cratio) {
	ratioF = cratio;
	rect.height = VIDEO_Y*ratioF;
	rect.width = VIDEO_X*ratioF;
}

void Camera::setRect(const RectEffect &newRect) {
	rect = newRect;
}
