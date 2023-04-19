#pragma once
#include <array>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>

#define CASE_SIZE 135
#define CASE_BAR 30
#define MRATE 44100
#define CASE_ALL 8*CASE_SIZE

#define VIDEO_Y CASE_ALL
#define VIDEO_X VIDEO_Y*16/9

#define START_POINT_X (VIDEO_X - CASE_ALL)/2
#define START_POINT_Y 0

#define DEFAULT_PATH_V "Ressources/video/"
#define DEFAULT_PATH_S "Ressources/video/AudioFiles/"
#define DEFAULT_PATH_E "Ressources/voices/"
#define DEFAULT_PATH_P "Ressources/files/"

void getType(const cv::Mat &ui);

/// @brief a Struct the set a rect of image
/// @author Thibaud VINCENT
struct RectEffect {
	std::size_t x;
	std::size_t y;
	std::size_t width;
	std::size_t height;
};


/// @brief When you do a chess move with 
/// @author Thibaud VINCENT
class ChessMove {
	public:
		int newX;
		int newY;
		int oldX;
		int oldY;
		bool eatP;
		int pieceEating;
		float currentX;
		float currentY;
		bool check;
		std::array<std::array<int, 8>, 8> chessArray;

		std::size_t getCenterX(bool bar) const;
		std::size_t getCenterY() const;
};

/// @brief To keep in mind sound time to play
/// @author Thibaud VINCENT
class PlaySound {
	public:
		std::string filename;
		std::size_t startT;
		std::size_t compteur;
		float volume;
		float timeStart;
		float timeEnd;

		PlaySound(std::string filename, std::size_t startT, float timeStart, float timeEnd, float volume);
		~PlaySound();

		void draw() const;
};

/// @brief the default namespace
/// @author Thibaud VINCENT
namespace Default {

	/// @brief remove the black of an image
	/// @param black the img
	/// @return void
	void removeBlack(cv::Mat black);

	/// @brief AlphaComposing to merge two image
	/// @param cv::Mat the background that will change
	/// @param cv::Mat the front to insert
	/// @param posX the y position
	/// @param posY the x position
	void alphaComposing(cv::Mat& back, const cv::Mat& front, std::size_t posX, std::size_t posY);

	/// @brief set Transparency
	/// @param piece pieces that i will set alpha
	/// @return cv::Mat
	cv::Mat setTransparency(const cv::Mat &piece, float alpha);

	/// @brief get a random value with the time
	/// @param start the start value
	/// @param end the end value random
	/// @return unsigned long
	unsigned long randomTime(unsigned long start, unsigned long end);

	/// Find the positon of a piece
	std::pair<int, int> findPieces(const int pieces, const std::array<std::array<int, 8>, 8> &chessArray);

} // namespace Default

