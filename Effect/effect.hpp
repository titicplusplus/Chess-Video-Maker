#pragma once
#include <future>
#include <opencv2/opencv.hpp>
#include "../default.hpp"
#include "../camera.hpp"

namespace Effect {

/// @brief The main class for the effect
/// @author Thibaud VINCENT
class Main {
	public:
		/// @brief the constructor
		Main();

		/// @brief the deconstructor
		virtual ~Main();

		/// @brief to set the effct for the video
		/// @param effect the image who will get the effect (CV_8UC3)
		virtual void setEffect(cv::Mat &effect, float fps, float numberFPS, const ChessMove &cmove, RectEffect &rect) {};

		/// @brief get time effect
		/// @param defaultFrameRate the frame rate of the video
		/// @return float the time effect
		virtual float getTimeEffect(float defaultFrameRate) const { return 0; };

	protected:
		Camera m_cam;
		std::future<void> openRessource;
};

} // namespace Effect
