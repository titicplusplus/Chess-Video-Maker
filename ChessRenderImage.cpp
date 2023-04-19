#include "ChessRenderImage.hpp"
#include <iostream>
#include "SaveMusic.hpp"
#include <ctime>
#include <future>
#include <cmath>
#include "MultiAccess.hpp"

using json = nlohmann::json;

int sign(double x) {
	return (x > 0) - (x < 0);
}

std::size_t RenderImage::numberOfFrames = 0;
std::vector<PlaySound> RenderImage::musicVec;
double RenderImage::FPS = 24;

RenderImage::RenderImage() : m_ref_piecesChess{piecesChess}, m_ref_chessArrayIDE{chessArrayIDE}, m_ref_backgroundChess{backgroundChess},
					m_ref_newMusicVec {newMusicVec}, m_ref_videoBackgroundChess {videoBackgroundChess} {
	const std::string themeName { "theme1" };
	backgroundChess = cv::imread("files/theme/" + themeName + "/board.png", cv::IMREAD_UNCHANGED);

	cv::cvtColor(backgroundChess, backgroundChess, cv::COLOR_RGB2RGBA);

	videoBackgroundChess = cv::imread("files/theme/" + themeName + "/background.jpg", cv::IMREAD_UNCHANGED);//cv::Mat(cv::Size(VIDEO_X, VIDEO_Y), CV_8UC4, cv::Scalar(0, 0, 0, 0));
	
	cv::cvtColor(videoBackgroundChess, videoBackgroundChess, cv::COLOR_RGB2RGBA);
	backgroundChess.copyTo(videoBackgroundChess(cv::Rect(START_POINT_X, START_POINT_Y, CASE_ALL, CASE_ALL)));

	piecesChess[0] = cv::imread("files/theme/" + themeName + "/wP.png", cv::IMREAD_UNCHANGED);
	piecesChess[1] = cv::imread("files/theme/" + themeName + "/wB.png", cv::IMREAD_UNCHANGED);
	piecesChess[2] = cv::imread("files/theme/" + themeName + "/wN.png", cv::IMREAD_UNCHANGED);
	piecesChess[3] = cv::imread("files/theme/" + themeName + "/wR.png", cv::IMREAD_UNCHANGED);
	piecesChess[4] = cv::imread("files/theme/" + themeName + "/wQ.png", cv::IMREAD_UNCHANGED);
	piecesChess[5] = cv::imread("files/theme/" + themeName + "/wK.png", cv::IMREAD_UNCHANGED);

	piecesChess[6] = cv::imread("files/theme/" + themeName + "/bP.png", cv::IMREAD_UNCHANGED);
	piecesChess[7] = cv::imread("files/theme/" + themeName + "/bB.png", cv::IMREAD_UNCHANGED);
	piecesChess[8] = cv::imread("files/theme/" + themeName + "/bN.png", cv::IMREAD_UNCHANGED);
	piecesChess[9] = cv::imread("files/theme/" + themeName + "/bR.png", cv::IMREAD_UNCHANGED);
	piecesChess[10] = cv::imread("files/theme/" + themeName + "/bQ.png", cv::IMREAD_UNCHANGED);
	piecesChess[11] = cv::imread("files/theme/" + themeName + "/bK.png", cv::IMREAD_UNCHANGED);

	//chessArray = reader.getArray();
	
	//chessData = puzzle.getPuzzle();

	//chessData = json::parse(R"({"game":{"id":"Sewe1gRB","perf":{"icon":"","name":"Classical"},"rated":true,"players":[{"userId":"alieov","name":"alieov","color":"white"},{"userId":"volkanov_sergey","name":"Volkanov_Sergey","color":"black"}],"pgn":"e4 c5 Nf3 a6 c3 d5 exd5 Qxd5 d4 Nf6 c4 Qe4+ Be3 Bg4 Nc3 Bxf3 gxf3 Qc6 d5 Qc7 Qa4+ Qd7 Nb5 axb5 Qxa8 Qc7 Bxc5 e6 Ba7","clock":"15+15"},"puzzle":{"id":"dbdmq","rating":1995,"plays":90330,"initialPly":28,"solution":["f8b4","e1d1","e8g8"],"themes":["middlegame","short","castling","advantage","defensiveMove"]}})");
		
	//????reader.setupPgnAdvance(chessData["game"]["pgn"].get<std::string>()); ?????????????

	//reader.setupPgnAdvance("e4 c5 Nf3 a6 c3 d5 exd5 Qxd5 d4 Nf6 c4 Qe4+ Be3 Bg4 Nc3 Bxf3 gxf3 Qc6 d5 Qc7 Qa4+ Qd7 Nb5 axb5 Qxa8 Qc7 Bxc5 e6 Ba7");
	//reader.setupPgnAdvance("e4 Nf6 e5 Ng8 d4 d6 Nf3 dxe5 Nxe5 Nd7 Bb5 c6 Ba4 Nxe5 dxe5 Qa5+ Nc3 Qxe5+ Be3 e6 O-O O-O-O ");
	//reader.setupPgnAdvance("O-O-O O-O-O ");

	//reader.openStringPgn(chessData["game"]["pgn"].get<std::string>());
	
	FPS = 24;//m_json["frameRate"];
}

RenderImage::~RenderImage() {}

/*
void RenderImage::addAudio(const std::string &file, float volume) {
	musicVec.push_back(PlaySound(file, (static_cast<float>(numberOfFrames)/FPS)*MRATE, volume));
}
*/

std::vector<PlaySound> RenderImage::getAudioVec() {
	return musicVec;
}

int RenderImage::piece2index(int piece) {
	int result {0};
	switch (piece%10) {
		case 1:
			result = 0;
			break;
		case 2:
			result = 3;
			break;
		case 3:
			result = 2;
			break;
		case 4:
			result = 1;
			break;
		case 5:
			result = 4;
			break;
		case 6:
			result = 5;
			break;
		default:
			result = -1;
			break;
	}

	result += static_cast<int>(piece/10) * 6;
	return result;
}

int RenderImage::getLisibleRating(double crate) {
	/**if (crate == 0) {
		return 1024*8*CASE_SIZE/2048;
	}

	return (1024 + sign(crate)*std::pow(2, std::abs(crate)))*8*CASE_SIZE/2048;**/

	const int maxValueSize { 1000 };


	if (crate > maxValueSize) {
		crate = maxValueSize;
	} else if (crate < -maxValueSize) {
		crate = -maxValueSize;
	}

	return std::round(CASE_SIZE*0.4 + 7.2*CASE_SIZE*(crate + maxValueSize)/(2*maxValueSize));
}

cv::Mat RenderImage::getBarWinner(float speed, float fps, float maxFPS, int oldRating) {
	cv::Mat bar { cv::Mat(cv::Size(CASE_BAR, 8*CASE_SIZE), CV_8UC4, cv::Scalar(255, 255, 255, 255)) };

	std::size_t pos { static_cast<std::size_t>(CASE_ALL - (oldRating+speed*fps)) };
	//std::cout << "The pos is " << pos  << " " << oldRating << " " << speed << " " << fps << "\n";

	if (pos != 0) {
		cv::Mat(cv::Size(CASE_BAR, pos), CV_8UC4, cv::Scalar(255, 83, 83, 255)).copyTo(bar(cv::Rect(0, 0, CASE_BAR, pos)));
	}

	return bar;

}

cv::Mat RenderImage::getFinalBarWinner(int newRating) {
	cv::Mat bar { cv::Mat(cv::Size(CASE_BAR, CASE_ALL), CV_8UC4, cv::Scalar(255, 255, 255, 255)) };

	std::size_t pos { static_cast<std::size_t>(8*CASE_SIZE - newRating) };

	if (pos != 0) {
		cv::Mat(cv::Size(CASE_BAR, pos), CV_8UC4, cv::Scalar(255, 83, 83, 255)).copyTo(bar(cv::Rect(0, 0, CASE_BAR, pos)));
	}

	//oldRating = newRating;
	return bar;
}

cv::Mat RenderImage::getImageVideo(cv::Mat bar, const cv::Mat &chess) {
	cv::Mat finalM = chess.clone();//{ cv::Mat(cv::Size(VIDEO_X, VIDEO_Y), CV_8UC4) };
	//chess.copyTo(finalM(cv::Rect(0, 0, VIDEO_X, VIDEO_Y)));
	bar.copyTo(finalM(cv::Rect(START_POINT_X - CASE_BAR, START_POINT_Y, CASE_BAR, CASE_ALL)));

	cv::cvtColor(finalM, finalM, cv::COLOR_RGBA2RGB);

	return finalM;
}

std::vector<cv::Mat> RenderImage::visualEntry() {
	std::vector<cv::Mat> video;
	cv::Mat square { cv::Mat(cv::Size(VIDEO_X, VIDEO_Y), CV_8UC3, cv::Scalar(0, 0, 0)) };
	std::size_t endTimeMove { static_cast<std::size_t>(timeMove*2) };

	float moveY { 2*CASE_SIZE/static_cast<float>(endTimeMove) };
	
	const float p1 {-2*CASE_SIZE + moveY*2};
	const float p2 {-CASE_SIZE + moveY*2};
	const float p3 {8*CASE_SIZE - moveY*2};
	const float p4 {9*CASE_SIZE - moveY*2};

	std::array<float, 32> yPos {p1, p1, p1, p1, p1, p1, p1, p1,
				p2, p2, p2, p2, p2, p2, p2, p2,
				p3, p3, p3, p3, p3, p3, p3, p3,
				p4, p4, p4, p4, p4, p4, p4, p4};

	for (std::size_t i {0}; i < endTimeMove; i++) {
		cv::Mat img = m_ref_videoBackgroundChess.get().clone();

		for (std::size_t j {0}; j < yPos.size(); j++) {
			int p = -1;
			if (j < 8) {
				if (j < 4) {
					p = piece2index(12 + j);
				} else if (j == 4) {
					p = 11;
				} else {
					p = piece2index(12 + (7 - j));
				}
			} else if (j < 16) {
				p = 6;
			} else if (j < 24) {
				p = 0;
			} else {
				if (j < 28) {
					p = piece2index(j - 22);
				} else if (j == 28) {
					p = 5;
				} else if (j == 29) {
					p = piece2index(4);
				} else if (j == 30) {
					p = piece2index(3);
				} else {
					p = piece2index(2);
				}

			}

			std::size_t posX {CASE_SIZE*(j%8)};
			int posY {static_cast<int>(std::lround(yPos[j]))};

			if (j < 16) {
				if (i == endTimeMove-1) {
					if (j < 8)
						posY = 0;
					else 
						posY = CASE_SIZE;
				}

				if (posY+CASE_SIZE > 0 && posY < 0) {
					Default::alphaComposing(img, piecesChess[p](cv::Rect(0, std::abs(posY), CASE_SIZE, CASE_SIZE + posY)), START_POINT_X + posX, 0);
				} else if (posY >= 0) {
					Default::alphaComposing(img, piecesChess[p], START_POINT_X + posX, posY);
				}

				yPos[j] += moveY;
			} else {
				if (i == endTimeMove-1) {
					if (j < 24)
						posY = 6*CASE_SIZE;
					else 
						posY = 7*CASE_SIZE;
				}

				if (posY < CASE_SIZE*8 && posY > 7*CASE_SIZE) {
					Default::alphaComposing(img, piecesChess[p](cv::Rect(0, 0, CASE_SIZE, 8*CASE_SIZE - posY)),
						START_POINT_X + posX, 8*CASE_SIZE - (8*CASE_SIZE - posY));

				} else if (posY <= CASE_SIZE*7) {
					Default::alphaComposing(img, piecesChess[p], START_POINT_X + posX, posY);
				}


				yPos[j] -= moveY;
			}

		}

		float alpha = 1 - static_cast<float>(i)/(endTimeMove);

		cv::Mat finalRender = getImageVideo(getFinalBarWinner(4*CASE_SIZE), img);
		cv::addWeighted(finalRender, 1 - alpha, square, alpha, 0.0, finalRender);

		video.push_back(finalRender);
		numberOfFrames++;
	}
				
	return video;
}

std::vector<cv::Mat> RenderImage::visualEnd(int newRating) {
	std::vector<cv::Mat> video;

	cv::Mat square { cv::Mat(cv::Size(VIDEO_X, VIDEO_Y), CV_8UC3, cv::Scalar(0, 0, 0)) };

	const cv::Mat chess = getChessImage(m_ref_chessArrayIDE.get().size() - 1);

	for (std::size_t i = 0; i < FPS*4; ++i) {
		double alpha { static_cast<double>(i)/(FPS*4) };
		cv::Mat finalRender = getImageVideo(getFinalBarWinner(newRating), chess);

		addWeighted(finalRender, 1 - alpha, square, alpha, 0.0, finalRender);

		video.push_back(finalRender);
		numberOfFrames++;
	}

	return video;
}

std::size_t RenderImage::getNumFrames() {
	return static_cast<float>(numberOfFrames * MRATE) / FPS;
}

void RenderImage::startIDE(const nlohmann::json &chessData) {
	chessArrayIDE.clear();
	ChessMove cmove;

	cmove.newX = -1;
	cmove.currentX = 0;
	cmove.currentY = 0;
	cmove.eatP = false;
	cmove.chessArray = reader.getArray();
	chessArrayIDE.push_back(cmove);

	//std::cout << chessData["game"]["pgn"] << std::endl;
	reader.setupPgnAdvance(chessData["game"]["pgn"].get<std::string>());

	std::size_t sizeFor { reader.getVector().size() };
	for (std::size_t i {0}; i < sizeFor; i++) {
		reader.advancePgnStep(cmove);
		chessArrayIDE.push_back(cmove);
	}

	cmove.currentX = 1;
	for (const auto &act : chessData["puzzle"]["solution"]) {
		cmove.oldX = act.get<std::string>()[0] - 'a';
		cmove.oldY = act.get<std::string>()[1] - '1';

		cmove.newX = act.get<std::string>()[2] - 'a';
		cmove.newY = act.get<std::string>()[3] - '1';

		if ((cmove.chessArray[cmove.oldX][cmove.oldY] == 6 || cmove.chessArray[cmove.oldX][cmove.oldY] == 16)
				&& std::abs(cmove.newX - cmove.oldX) > 1) {
			if (cmove.newX == 3) {
				cmove.newY = 0;
			} else {
				cmove.newY = 1;
			}

			if (cmove.chessArray[cmove.oldX][cmove.oldY] == 5) {
				cmove.newX = 9;
			} else {
				cmove.newX = 10;
			}

			cmove.eatP = false;
		} else if (cmove.chessArray[cmove.newX][cmove.newY] != 0) {
			cmove.eatP = true;
			cmove.pieceEating = cmove.chessArray[cmove.newX][cmove.newY];
		} else {
			cmove.eatP = false;
			cmove.pieceEating = 0;
		}

		cmove.chessArray[cmove.newX][cmove.newY] = cmove.chessArray[cmove.oldX][cmove.oldY];
		cmove.chessArray[cmove.oldX][cmove.oldY] = 0;

		chessArrayIDE.push_back(cmove);
		cmove.currentX = 0;
	}

}

std::vector<cv::Mat> RenderImage::getMoveIDE(nlohmann::json &cmd, std::size_t moveNbr, int moveType,
		const std::size_t frameRate, const std::size_t defaultTime) {
	if (moveType > 0)
		moveNbr++;

	FPS = frameRate;
	timeMove = defaultTime;

	numberOfFrames = 0;
	musicVec.clear();

	const int newRating = 4*CASE_SIZE;

	cv::Mat start = getChessImage(moveNbr);
	if (moveType == 0 || moveType == 2) {
		if (cmd.size() == 0) {
			return {getImageVideo(getFinalBarWinner(newRating), start)};
		} else {
			return effectIDE(moveNbr, start, cmd, frameRate, defaultTime, 0, 4, newRating );
		}
	} else {
		std::vector<cv::Mat> video;
		transitionIDE(moveNbr, start, video, cmd, frameRate, defaultTime, 0, 4, newRating, newRating );
		return video;
	}
}

std::size_t RenderImage::getNbrMoveIDE() {
	return chessArrayIDE.size();
}

cv::Mat RenderImage::getChessImage(const std::size_t cmoveIndex) {
	cv::Mat chess;// = m_ref_videoBackgroundChess.get().clone();
	m_ref_videoBackgroundChess.get().copyTo(chess);

	const auto chessArray { m_ref_chessArrayIDE.get()[cmoveIndex].chessArray };

	for (std::size_t x = 0; x < 8; ++x) {
		for (int y = 0; y < 8; ++y) {
			if (chessArray[x][y] != 0) {
				int index { piece2index(chessArray[x][y]) };

		Default::alphaComposing(chess, m_ref_piecesChess.get()[index], START_POINT_X + x*CASE_SIZE, START_POINT_Y + (7 - y)*CASE_SIZE);
			}
		}
	}

	//cv::Mat finalRender = getImageVideo(getFinalBarWinner(), chess);
	return chess;
}

std::vector<cv::Mat> RenderImage::effectIDE(const std::size_t cmoveIndex, cv::Mat &chess, const nlohmann::json &cmd,
				std::size_t frameRate, std::size_t defaultTime, const std::size_t currentIndex,
				const std::size_t type, int newRating) {
	//const std::string key { m_json.getKey(index, actType) };
	std::vector<cv::Mat> video;
	JsonCommand jsonEffect {cmd};
	
	ChessMove aloneCmove {m_ref_chessArrayIDE.get()[cmoveIndex]};

	std::size_t indexAll {0};

	for (std::size_t i {0}; i < cmd.size(); i++) {
		jsonEffect.setTransitionIDE(cmd[i], frameRate, defaultTime);

		std::size_t timeEffect { jsonEffect.getTimeEffect() };//key, i)+10 };
		bool cameraWhen { jsonEffect.getCameraBeforeSetup() };//key, i) };

		for (std::size_t j {0}; j < timeEffect; j++) {

			cv::Mat chessEffect { chess.clone() };
			cv::Mat finalRender = getImageVideo(getFinalBarWinner(newRating), chessEffect);

			setFrameTransitionSpeJson(jsonEffect, finalRender, cameraWhen, timeEffect, aloneCmove, j, indexAll, currentIndex, type);

			video.push_back(finalRender);
			numberOfFrames++;

			indexAll++;
		}
	}

	return video;
}

void RenderImage::transitionIDE(const std::size_t cmoveIndex, cv::Mat &cBack, std::vector<cv::Mat> &video, const nlohmann::json &cmd,
		std::size_t frameRate, double defaultTime, const std::size_t currentIndex, const std::size_t type,
		int newRating, int oldRating) {

	JsonCommand jsonEffect {cmd};

	ChessMove cmove {m_ref_chessArrayIDE.get()[cmoveIndex]};

	int oldYR = 7 - cmove.oldY;
	int newYR = 7 - cmove.newY;
	double moveX { static_cast<double>(cmove.newX - cmove.oldX)*CASE_SIZE/defaultTime };
	double moveY { static_cast<double>(cmove.newY - cmove.oldY)*-CASE_SIZE/defaultTime };

	double barMove { static_cast<double>(newRating - oldRating)/defaultTime };

	int result {-1};
	int pieceE {-1};

	if (cmove.newX == 9) {
		m_ref_backgroundChess.get()(cv::Rect(4*CASE_SIZE, 7*CASE_SIZE, CASE_SIZE, CASE_SIZE)).copyTo(
				cBack(cv::Rect(START_POINT_X  + 4*CASE_SIZE, START_POINT_Y + 7*CASE_SIZE, CASE_SIZE, CASE_SIZE)));

		m_ref_backgroundChess.get()(cv::Rect(cmove.newY*7*CASE_SIZE, 7*CASE_SIZE, CASE_SIZE, CASE_SIZE)).copyTo(
				cBack(cv::Rect(START_POINT_X + cmove.newY*7*CASE_SIZE, START_POINT_Y + 7*CASE_SIZE, CASE_SIZE, CASE_SIZE)));

		result = 5;
		pieceE = 3;

		if (cmove.newY == 0) {
			moveX = -2*CASE_SIZE/defaultTime;
			moveY = 3*CASE_SIZE/defaultTime;
		} else {
			moveX = 2*CASE_SIZE/defaultTime;
			moveY = -2*CASE_SIZE/defaultTime;
		}
	} else if (cmove.newX == 10) {
		m_ref_backgroundChess.get()(cv::Rect(4*CASE_SIZE, 0, CASE_SIZE, CASE_SIZE)).copyTo(
				cBack(cv::Rect(START_POINT_X + 4*CASE_SIZE, START_POINT_Y, CASE_SIZE, CASE_SIZE)));

		m_ref_backgroundChess.get()(cv::Rect(cmove.newY*7*CASE_SIZE, 0, CASE_SIZE, CASE_SIZE)).copyTo(
				cBack(cv::Rect(START_POINT_X + cmove.newY*7*CASE_SIZE, START_POINT_Y, CASE_SIZE, CASE_SIZE)));
		result = 11;
		pieceE = 9;


		if (cmove.newY == 0) {
			moveX = -2*CASE_SIZE/defaultTime;
			moveY = 3*CASE_SIZE/defaultTime;
		} else {
			moveX = 2*CASE_SIZE/defaultTime;
			moveY = -2*CASE_SIZE/defaultTime;
		}
	} else {
		m_ref_backgroundChess.get()(cv::Rect(cmove.oldX*CASE_SIZE, oldYR*CASE_SIZE, CASE_SIZE, CASE_SIZE)).copyTo(
				cBack(cv::Rect(START_POINT_X + cmove.oldX*CASE_SIZE, START_POINT_Y + oldYR*CASE_SIZE, CASE_SIZE, CASE_SIZE)));

		m_ref_backgroundChess.get()(cv::Rect(cmove.newX*CASE_SIZE, newYR*CASE_SIZE, CASE_SIZE, CASE_SIZE)).copyTo(
				cBack(cv::Rect(START_POINT_X + cmove.newX*CASE_SIZE, START_POINT_Y + newYR*CASE_SIZE, CASE_SIZE, CASE_SIZE)));

		result = piece2index(cmove.chessArray[cmove.newX][cmove.newY]);
		pieceE = piece2index(cmove.pieceEating);
	}

	bool effect {false};
	bool cameraWhen;

	if (cmd.size() != 0) {
		effect = true;
		jsonEffect.setTransitionIDE(cmd[0], frameRate, defaultTime);
		cameraWhen = jsonEffect.getCameraBeforeSetup();
	}

	cv::Mat currentV = cBack.clone();


	for (float fps {0}; fps < defaultTime-1; ++fps) {
		currentV = cBack.clone();

		cv::Mat finalRender;

		if (cmove.newX < 9) {
			if (cmove.eatP && fps < defaultTime*0.80 && pieceE > -1) {
				Default::alphaComposing(currentV, Default::setTransparency(m_ref_piecesChess.get()[pieceE], 1 - fps/defaultTime*0.80), 
						START_POINT_X + cmove.newX*CASE_SIZE, START_POINT_Y + newYR*CASE_SIZE);
			}


			cmove.currentX = START_POINT_X + cmove.oldX*CASE_SIZE + moveX*fps;
			cmove.currentY = START_POINT_Y + oldYR*CASE_SIZE + moveY*fps;

			Default::alphaComposing(currentV, m_ref_piecesChess.get()[result], cmove.currentX, cmove.currentY);

			finalRender = getImageVideo(getBarWinner(barMove, fps, 0, oldRating), currentV);
			setFrameTransitionSpeJson(jsonEffect, finalRender, cameraWhen, defaultTime, cmove, fps, fps, currentIndex, type, effect);

			video.push_back(finalRender.clone());
			numberOfFrames++;

		} else if (cmove.newX < 11) {
			Default::alphaComposing(currentV, m_ref_piecesChess.get()[result], START_POINT_X + 4*CASE_SIZE + moveX*fps,
				       	START_POINT_Y + (7 - (cmove.newX-9)*7)*CASE_SIZE);

			Default::alphaComposing(currentV, m_ref_piecesChess.get()[pieceE], START_POINT_X + cmove.newY*7*CASE_SIZE + moveY*fps,
				       	START_POINT_Y + (7 - (cmove.newX-9)*7)*CASE_SIZE);

			cmove.currentX = START_POINT_X + 4*CASE_SIZE + moveX*fps;
			cmove.currentY = START_POINT_Y + (7 - (cmove.newX-9)*7)*CASE_SIZE;

			cv::Mat finalRender {getImageVideo(getBarWinner(barMove, fps, 0, oldRating), currentV)};

			setFrameTransitionSpeJson(jsonEffect, finalRender, cameraWhen, defaultTime, cmove, fps, fps, currentIndex, type, effect);
			
			video.push_back(finalRender.clone());
			numberOfFrames++;
		}

	}

	if (cmove.newX < 9) {
		Default::alphaComposing(cBack, m_ref_piecesChess.get()[result], START_POINT_X + cmove.newX*CASE_SIZE, START_POINT_Y + newYR*CASE_SIZE);
	} else {
		if (cmove.newY == 0) {
			Default::alphaComposing(cBack, m_ref_piecesChess.get()[result], START_POINT_X + 2*CASE_SIZE,
				START_POINT_Y + (7 - (cmove.newX-9)*7)*CASE_SIZE);

			Default::alphaComposing(cBack, m_ref_piecesChess.get()[pieceE], START_POINT_X + 3*CASE_SIZE,
				START_POINT_Y + (7 - (cmove.newX-9)*7)*CASE_SIZE);
		} else {
			Default::alphaComposing(cBack, m_ref_piecesChess.get()[result], START_POINT_X + 6*CASE_SIZE,
				START_POINT_Y + (7 - (cmove.newX-9)*7)*CASE_SIZE);

			Default::alphaComposing(cBack, m_ref_piecesChess.get()[pieceE], START_POINT_X + 5*CASE_SIZE,
				START_POINT_Y + (7 - (cmove.newX-9)*7)*CASE_SIZE);
		}
	}


	cv::Mat finalRender = getImageVideo(getFinalBarWinner(newRating), cBack);
	setFrameTransitionSpeJson(jsonEffect, finalRender, cameraWhen, defaultTime, cmove, defaultTime-1, defaultTime-1, currentIndex, type, effect);
	video.push_back(finalRender.clone());
	numberOfFrames++;

	if (cmove.eatP && pieceE > -1 && cmove.newX < 9) {
		if (type == 4) {
			musicVec.push_back(PlaySound("files/sound/Capture.ogg", (static_cast<float>(numberOfFrames)/FPS)*MRATE, 0, -1, 0.5));
		} else {
			m_ref_newMusicVec.get()[currentIndex*3 + type].push_back(PlaySound("files/sound/Capture.ogg", defaultTime-1, 0, -1, 0.5));
		}
	} else {
		if (type == 4) {
			musicVec.push_back(PlaySound("files/sound/Move.ogg", (static_cast<float>(numberOfFrames)/FPS)*MRATE, 0, -1, 0.5));
		} else {
			m_ref_newMusicVec.get()[currentIndex*3 + type].push_back(PlaySound("files/sound/Move.ogg", defaultTime-1, 0, -1, 0.5));
		}
	}

}

void RenderImage::setFrameTransitionSpeJson(JsonCommand &jsonEffect, cv::Mat &img, bool cameraWhen, int timeEffect, const ChessMove &cmove,
		size_t index, std::size_t indexAll, const std::size_t currentIndex, const std::size_t type, bool effect) {

	if (!effect) {
		return;
	}

	RectEffect m_rect = jsonEffect.getCamera(img.size(), index, cmove, timeEffect-1);

	cam.setRect(m_rect);

	if (cameraWhen) {
		cam.zoomPosPoint(img, m_rect.x, m_rect.y);
	}

	bool audio = jsonEffect.setEffect(img, index, timeEffect, m_rect.x + m_rect.width/2,
			m_rect.y + m_rect.height/2, cmove, jsonEffect.getCurrentRatio(index, timeEffect), currentIndex, type);

	if (audio) {
		auto audioI {jsonEffect.audioInformation()};
		auto timeI  {jsonEffect.timeInformation()};
		if (type == 4) {
			musicVec.push_back(PlaySound(audioI.first , (static_cast<float>(numberOfFrames)/FPS)*MRATE, timeI.first, timeI.second, audioI.second));
		} else {
			m_ref_newMusicVec.get()[currentIndex*3 + type].push_back(PlaySound(audioI.first, indexAll, timeI.first, timeI.second, audioI.second));
		}
	}

	if (!cameraWhen) {
		cam.zoomPosPoint(img, m_rect.x, m_rect.y);
	}
}

size_t RenderImage::sizeCmoveIDE() {
	return chessArrayIDE.size();
}


void RenderImage::fastRenderAsync(size_t startIndex, size_t endIndex, const nlohmann::json &renderJson,
		std::vector<std::vector<cv::Mat>> &renderVec, const std::string videoName) {

	using namespace std::chrono_literals;

	//std::atomic_ref<nlohmann::json> json_secure {renderJson};

	const size_t maxFuture {3};
	size_t currentIndex {startIndex};
	int type = 0;

	renderVec.clear();
	//renderVec.insert(v.end()(endIndex - currentIndex + 1)*3, {});
	
	for (size_t i {0}; i < (endIndex - currentIndex)*3; i++) {
		newMusicVec.push_back({});
		renderVec.push_back({});
	}

	// +2
	renderVec.push_back({});
	renderVec.push_back({});

	std::vector<std::pair<std::future<std::vector<cv::Mat>>, int>> futureRenderMove;

	futureRenderMove.push_back(std::make_pair(std::async(//std::launch::async,
			std::bind(&RenderImage::visualEntry, this)), 0));
					

	for (std::size_t i {1}; i < maxFuture && currentIndex < endIndex; i++) {
		futureRenderMove.push_back(std::make_pair(std::async(
			std::bind(&RenderImage::renderImage, this, currentIndex, type, renderJson)),
					currentIndex * 3 + type + 1));

		type++;

		if (type == 3) {
			currentIndex++;
			type = 0;
		}

		//valueToVector[i] = i;
	}

	bool continueWork {true};

	FPS = renderJson["render"]["frameRate"];
	timeMove = renderJson["render"]["defaultTime"];

	numberOfFrames = 0;
	musicVec.clear();

	//newRating = 4*CASE_SIZE;
	//oldRating = 4*CASE_SIZE;

	std::condition_variable cv;
	std::mutex mu_render;

   	time_t start {time(0)};

	std::thread workerVideoWriter {&RenderImage::writeAsyncVideo, &renderVec, &cv, &mu_render, videoName};

	std::unique_lock<std::mutex> render_lck { mu_render };
	render_lck.unlock();

	std::vector<size_t> timeByIndex;

	while (continueWork) {
		if (futureRenderMove.size() == 0) {
			break;
		}

		for (auto it = futureRenderMove.begin(); it != futureRenderMove.end();) {
			if (it->first.wait_for(10ms) == std::future_status::ready) {
				render_lck.lock();

				renderVec[it->second] = it->first.get();

				if (renderVec[it->second].empty()) {
					renderVec[it->second].push_back(
				renderVec[it->second - 1][renderVec[it->second - 1].size() - 1].clone()
					);
				}

					
				timeByIndex.push_back(renderVec[it->second].size());
				render_lck.unlock();
				cv.notify_one();

				if (currentIndex == endIndex) {
					if (type != 4) {
						(*it) = std::make_pair( std::async(std::launch::async,
								std::bind(&RenderImage::visualEnd, this,
				getLisibleRating(renderJson["automaker"][renderJson["automaker"].size() - 2]["moveplayed_cp"]))),
						       		currentIndex * 3 + type + 1);
						type = 4;
	
					} else {
						futureRenderMove.erase(it);
					}
				} else {
					(*it) = std::make_pair( std::async(std::launch::async,
							std::bind(&RenderImage::renderImage, this, currentIndex, type,
								renderJson)),
						       	currentIndex * 3 + type + 1);
					
					type++;

					if (type == 3) {
						currentIndex++;
						type = 0;
					}

					++it;

					//continueWork = false;
				}
			}
		}
	}


	std::cout << timeByIndex.size() << " versus " << renderVec.size() << "\n";
	std::thread workerAudioWriter {&RenderImage::renderAudio, newMusicVec, timeByIndex, renderJson["render"]["frameRate"]};

	std::cout << "Waiting async audio writer" << std::endl;
	workerAudioWriter.join();
	std::cout << "Waiting async video writer" << std::endl;
	workerVideoWriter.join();

   	time_t end = time(0);

	std::cout << "The result is: " << end - start << std::endl;
}

void RenderImage::writeAsyncVideo(std::vector<std::vector<cv::Mat>> *renderVec, std::condition_variable *cv,
				std::mutex *mu_render, const std::string videoName) {
	size_t videoIndex {0};

	cv::VideoWriter video(videoName, cv::VideoWriter::fourcc('M','J','P','G'), FPS,
			cv::Size(VIDEO_X, VIDEO_Y));

	video.set(cv::CAP_PROP_FPS, FPS);

	while (videoIndex < renderVec->size()) {
		{
			std::unique_lock<std::mutex> lck { *mu_render };
			cv->wait(lck, [videoIndex, renderVec] {
					return (*renderVec)[videoIndex].size();
			});
		}

		for (const auto &img : (*renderVec)[videoIndex]) {
			video.write(img);
		}
	
		(*renderVec)[videoIndex].clear();
		videoIndex++;

		std::cout << "Video Index: -> " << videoIndex << "/" << renderVec->size() << std::endl;
	}

	video.release();
	return;
}


void RenderImage::renderAudio(const std::vector<std::vector<PlaySound>> &playSoundVec, const std::vector<size_t> &renderVec,
		const std::size_t frameRate) {
	std::vector<PlaySound> finalAudio;

	// (static_cast<float>(numberOfFrames)/FPS)*MRATE
	
	size_t timeAudio {renderVec[0]};

	for (std::size_t index {1}; index < renderVec.size() - 1; index++) {
		for (const auto &playmusic : playSoundVec[index - 1]) {
			finalAudio.push_back(PlaySound(playmusic.filename,
					(static_cast<float>(timeAudio + playmusic.startT)/frameRate)*MRATE,
						playmusic.timeStart, playmusic.timeEnd,
						playmusic.volume));

		}
			
		timeAudio += renderVec[index];
	}


	timeAudio += renderVec[renderVec.size() - 1];

	timeAudio = static_cast<double>(timeAudio) * (MRATE / frameRate);

	SaveMusic mus;
	std::cout << "Render audio part ! " << timeAudio << "\n";
	if (mus.createAudioFile(finalAudio, timeAudio)) {
		std::cout << "Write video" << "\n";
		mus.saveToFile("yeah.wav");
	}
}

std::vector<cv::Mat> RenderImage::renderImage(const std::size_t currentIndex, const std::size_t type,
				const nlohmann::json &renderJson) {

	//std::cout << "P1\n";
	const std::size_t cmoveIndex {currentIndex + (type == 0 ? 0 : 1) };
	const std::string key { JsonCommand::getKeyIndex(currentIndex, type) };

	//std::cout << "P2\n";
	std::vector<cv::Mat> video;

	//std::cout << "P3\n";
	cv::Mat start = getChessImage(cmoveIndex);

	int newRating;
       
	if (cmoveIndex) {
		if (cmoveIndex < renderJson["automaker"].size() && renderJson["automaker"][cmoveIndex - 1].contains("moveplayed_cp"))
			newRating = {getLisibleRating(renderJson["automaker"][cmoveIndex - 1]["moveplayed_cp"])};
		else {
			newRating = {getLisibleRating(renderJson["automaker"][renderJson["automaker"].size() - 2]["moveplayed_cp"])};
		}
	} else {
		newRating = 4*CASE_SIZE;
	}

	//{getLisibleRating(renderJson["automaker"][currentIndex]["moveplayed_cp"])};

	if (type != 1) {
		if (!renderJson["render"].contains(key) || renderJson["render"][key].size() == 0) {
			//std::cout << "P4\n";
			return {getImageVideo(getFinalBarWinner(newRating), start)};
		} else {
			//std::cout << "P5\n";
			return effectIDE(cmoveIndex, start, renderJson["render"][key],
					renderJson["render"]["frameRate"], renderJson["render"]["defaultTime"],
					currentIndex, type, newRating);
		}
	} else {
		//std::cout << "P6\n";
		const json real = json::array();
		std::reference_wrapper<const json> ui = std::cref(real);
		if (renderJson["render"].contains(key)) {
			ui = std::cref(renderJson["render"][key]);
		}
	
		int oldRating;

		if (cmoveIndex > 1) {
			oldRating = getLisibleRating(renderJson["automaker"][cmoveIndex - 2]["moveplayed_cp"]);
		} else {
			oldRating = 4*CASE_SIZE;
		}

		//std::cout << "P7\n";
		transitionIDE(cmoveIndex, start, video,
				ui,//renderJson[key],
				renderJson["render"]["frameRate"],
				static_cast<double>(renderJson["render"]["defaultTime"].get<int>()),
				currentIndex, type, newRating, oldRating);//, frameRate, defaultTime);
		return video;
	}

	return {};
}


const std::vector<ChessMove> &RenderImage::getChessMove() {
	return chessArrayIDE;
}
