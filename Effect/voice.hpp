#pragma once
#include <nlohmann/json.hpp>

/**#include <curlpp/cURLpp.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Easy.hpp>**/

#include <vector>

#include "effect.hpp"
#include "../default.hpp"
#include "../camera.hpp"

namespace Effect {

size_t writefunc(void *ptr, size_t size, size_t nmemb, std::string *s);

class Voice : public Main {
	public:
		Voice(const std::string text, int colorPlayer);
		~Voice();
		
		/// @brief to set the effct for the video
		/// @param effect the image who will get the effect (CV_8UC3)
		virtual void setEffect(cv::Mat &effect, float fps, float numberFPS, const ChessMove &cmove, RectEffect &rect);

		void getSizeText();

		void setTimeEffect();

		/// @brief get time effect
		/// @return float the time effect
		virtual float getTimeEffect(float defaultFrameRate) const;

	private:
		const std::string m_text;
		const std::string filenameWAV;
		int m_c;

		float m_timeEffect;

		std::size_t m_textSize;

		cv::Scalar m_scalar;
};

} // namespace Effect
