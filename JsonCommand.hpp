#pragma once
#include "Effect/effect.hpp"
#include "Effect/voice.hpp"
#include "Effect/GreenBackground.hpp"

#include <nlohmann/json.hpp>
#include <memory>

/// @brief the goal of this class is to open a JSON file and execute command
/// @author Thibaud VINCENT
class JsonCommand {
	public:
		/// @brief the action type it's to know when pass action
		enum ActionType : char {
			/// @brief for the beginning
			START = 0,
			/// @brief for the middle
			MIDDLE = 1,
			/// @brief for the end
			END = 2
		};

		/// @brief the camera type
		enum CamType : signed char {
			/// @brief When the camera is None,
			NONE = -1,
			/// @brief When the camera is setup with transition and manual
			TR_MANUAL = 0,
			/// @brief When the camera is setup manually and static
			STATIC_MANUAL = 1,
			/// @brief When the camera track the piece
			TRACK_MOVE = 2,
			/// @brief When the camera is setup to see all the movement
			ALL_MOVE = 3,
			/// @brief When the camera is setup to see the final case
			FINAL_MOVE = 4
		};

		/// @brief the constructor, open the config.json file
		JsonCommand(const nlohmann::json &cmd);

		/// @brief get an argument
		/// @param cmd the key of json args
		/// @return json value
		//const nlohmann::json &operator[](const std::string& cmd) const;
	
		/// @brief get the camera type before setup
		/// @param the key
		/// @parma index the current index
		/// @return CamType
		CamType getCameraTypeSetup() const;//const std::string &key, const std::size_t &index) const;

		/// @brif setup a new camera and effect with new transition
		/// @return void
		void setupNewTrCameraEffect();

		/// @brief the RectSize camera
		/// @param imgSize the cv::Size of cv::Mat
		/// @param currentFrame the current frame number on the video
		/// @param timeFrame
		/// @param cmove the chess move
		/// @return RectEffect
		RectEffect getCamera(const cv::Size &imgSize, const std::size_t currentFrame, const ChessMove &cmove,
				std::size_t timeFrame = 0);

		/// @brief get time of the effect
		/// @param the key
		/// @param index the current index
		/// @return float
		const std::size_t getTimeEffect() const;//const std::string &key, const std::size_t &index) const;

		/// @brief set effect
		bool setEffect(cv::Mat &effect, float fps, float numberFPS,
				double centerX, double centerY, const ChessMove &cmove, double currentRatio,
				const std::size_t currentIndex, const std::size_t type);

		/// @@brief get key
		/// @param index the current index
		/// @param indexAction the action type
		/// @return const std::string
		static const std::string getKeyIndex(std::size_t index, std::size_t indexAction);

		/// @brief get time how to setup the camera
		/// @param key the key to access data in json
		/// @param index the current index
		/// @return bool
		const bool getCameraBeforeSetup() const;//const std::string &key, const std::size_t &index) const;

		/// @brief get current ratio
		/// @param index the current index
		/// @param timeEffect the time of the effect
		/// @return float
		float getCurrentRatio(float index, float timeEffect);

		/// @brief set transition with one json (IDE process)
		/// @return void
		void setTransitionIDE(const nlohmann::json &cmd, std::size_t frameRate, std::size_t defaultTime);

		/// @brief return audio information
		/// @return std::pair(std::string, float volume)
		std::pair<std::string, float> audioInformation();
		
		/// @brief return time information
		/// @return return time information
		std::pair<double, double> timeInformation();

	private:
		std::reference_wrapper<const nlohmann::json> currentProcess;

		std::vector<double> m_camera;
		CamType m_cameraType;

		std::size_t timeEffect;

		std::unique_ptr<Effect::Main> m_effect;

		std::string m_fileSound;
		float m_volumeSound;
		int m_typeSound;
		float m_timeSound;

		std::size_t frameRate;
		std::size_t defaultTime;
};
