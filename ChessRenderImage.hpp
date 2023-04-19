#pragma once
#include <opencv2/opencv.hpp>
#include <condition_variable>
#include <nlohmann/json.hpp>
#include <array>
#include <memory>
#include <atomic>

#include "JsonCommand.hpp"
#include "camera.hpp"
#include "getpuzzle.hpp"
#include "CsvReader.hpp"
//#include "OpenMusic.hpp"
#include "ChessParser.hpp"

#include "default.hpp"

#include "MultiAccess.hpp"

int sign(double x);

/// @brief Class to convert chess format to OpenCV image
/// @author Thibaud VINCENT
class RenderImage {
	public:
		/// @brief Class Constructor
		RenderImage();

		/// @brief Class Destructor
		~RenderImage();

		/// @brief The start the IDE-PLAN
		void startIDE(const nlohmann::json &chessData);

		/// @brief get size of cmove
		/// @return size_t
		size_t sizeCmoveIDE();

		/// @brief Make the render
		/// @param cmd the json file
		/// @param moveNbr the moveNbr
		/// @param moveType the type of the move
		/// @return std::vector<cv::Mat> (to make a video of cv::Mat)
		std::vector<cv::Mat> getMoveIDE(nlohmann::json &cmd, std::size_t moveNbr, int moveType,
		const std::size_t frameRate, const std::size_t defaultTime);
		
		/// @brief to init transition between two step
		/// @param cmove the current ChessMove
		/// @param cv::Mat of the last transition
		/// @return void
		void transitionIDE(const std::size_t cmoveIndex, cv::Mat &cBack, std::vector<cv::Mat> &video,
				const nlohmann::json &cmd, std::size_t frameRate, double defaultTime,
				const std::size_t currentIndex, const std::size_t type, int newRating, int oldRating);

		/// @brief to set effect when there is no move
		/// @param cmove the current ChessMove
		/// @param cv::Mat of the last transition
		/// @return void
		std::vector<cv::Mat> effectIDE(const std::size_t cmoveIndex, cv::Mat &chess, const nlohmann::json &cmd,
				std::size_t frameRate, size_t defaultTime,
				const std::size_t currentIndex, const std::size_t type, int newRating);

		/// @brief render a part of video
		/// @param cmove the ChessMove
		/// @param key the std::string key
		/// @param renderJson
		/// @return std::vector<cv::Mat>
		std::vector<cv::Mat> renderImage(const std::size_t currentIndex, std::size_t type,
				const nlohmann::json &renderJson);
	
		/// @brief fast render function (std::async)
		/// @param startIndex the start
		/// @param endIndex the end
		/// @param renderJson the json value
		/// @param renderVec the std::vector<cv::Mat>
		/// @param videoAction the video action
		/// @return void
		void fastRenderAsync(size_t startIndex, size_t endIndex, const nlohmann::json &renderJson,
				std::vector<std::vector<cv::Mat>> &renderVec, const std::string videoName);


		/// @brief async video render
		/// @param renderVec std::vector<std::vector<cv::Mat>>
		/// @param cv the condition_variable
		/// @param mu_render the std::mutex
		static void writeAsyncVideo(std::vector<std::vector<cv::Mat>> *renderVec, std::condition_variable *cv,
				std::mutex *mu_render, const std::string videoName);

		/// @brief async audio render
		/// @param renderJson the json files
		static void renderAudio(const std::vector<std::vector<PlaySound>> &playSoundVec,
				const std::vector<size_t> &renderVec,
				const std::size_t frameRate);

		/// @brief get number of movement:
		/// @return std::size_t the number of move
		std::size_t getNbrMoveIDE();

		/// @brief get chess img
		/// @param cmove the chessmove cmove
		/// @return cv::Mat
		cv::Mat getChessImage(const std::size_t cmoveIndex);

		/// @brief visual entry
		std::vector<cv::Mat> visualEntry();

		/// @brief set a visual End 
		std::vector<cv::Mat> visualEnd(int newRating);

		/// @brief get piece index
		/// @param piece the piece
		/// @return int (index of piecesChess)
		int piece2index(int piece);

		/// @brief get the bar of win rate
		/// @return cv::Mat
		cv::Mat getBarWinner(float speed, float fps, float maxFPS, int oldRating);

		/// @brief get the final bar of win rate
		/// @return cv::Mat
		cv::Mat getFinalBarWinner(int newRating);

		/// @brief get final video image
		/// @param bar the bar of the video
		/// @parma chess the chess board
		/// @return cv::mat
		cv::Mat getImageVideo(cv::Mat bar, const cv::Mat &chess);

		/// @brief get a lisible rating
		/// @param the current rating
		/// @return double
		int getLisibleRating(double crate);

		/// @brief set frame transition
		/// @param jsonEffect the jsoncommand effect
		/// @param img the cv::Mat img to set effect
		/// @param cameraWhen if position is absolute or relative
		/// @param timeEffect
		/// @param cmove the chess move value
		/// @param index the current index
		/// @param effect if can do it
		/// @return void
		void setFrameTransitionSpeJson(JsonCommand &jsonEffect, cv::Mat &img, bool cameraWhen, int timeEffect, const ChessMove &cmove,
					size_t index, std::size_t indexAll, const std::size_t currentIndex, const std::size_t type, bool effect = true);

		/**
		/// @brief add new audio
		/// @param file the filename
		/// @param volume the volume
		static void addAudio(const std::string &file, float volume);
		**/

		/// @brief get the audio vector
		static std::vector<PlaySound> getAudioVec();

		/// @brief get number of frames
		/// @return get frames
		static std::size_t getNumFrames();

		/// @brief get Chessmove
		/// @return const std::vector<ChessMove> &
		const std::vector<ChessMove> &getChessMove();



	private:
		std::array<cv::Mat, 12> piecesChess;
		MultiAccess<std::array<cv::Mat, 12>> m_ref_piecesChess;

		ChessParser reader;

		std::array<std::array<int , 8>, 8> chessArray;

		//OpenMusic music;
		int timeMove;

		Camera cam;

		std::vector<ChessMove> chessArrayIDE;
		MultiAccess<std::vector<ChessMove>> m_ref_chessArrayIDE;

		static std::vector<PlaySound> musicVec;

		std::vector<std::vector<PlaySound>> newMusicVec;
		MultiAccess<std::vector<std::vector<PlaySound>>> m_ref_newMusicVec;

		static std::size_t numberOfFrames;

		static double FPS;
		std::mutex m_changeCmove;

		cv::Mat backgroundChess;
		MultiAccess<cv::Mat> m_ref_backgroundChess;

		cv::Mat videoBackgroundChess;
		MultiAccess<cv::Mat> m_ref_videoBackgroundChess;

		std::mutex m_readBackground;
};
