#include "JsonCommand.hpp"
//#include "ChessRenderImage.hpp"
#include <fstream>

using json = nlohmann::json;

JsonCommand::JsonCommand(const nlohmann::json &cmd) : currentProcess {std::ref(cmd)} {
	m_fileSound = "";
	m_typeSound = -1;
	m_volumeSound = 0;
	m_timeSound = 0;

	frameRate = 0;
	defaultTime = 0;
}

JsonCommand::CamType JsonCommand::getCameraTypeSetup() const {//const std::string &key, const std::size_t &index) const {
	const json &cam = currentProcess.get()["cam"];

	if (cam.size() == 3) {
		return STATIC_MANUAL;
	} else if (cam.size() == 6) {
		return TR_MANUAL;
	} else if (cam.size() == 2) {
		if (cam[0] == 0) {
			return TRACK_MOVE;
		} else if (cam[0] == 1) {
			return ALL_MOVE;
		} else if (cam[0] == 2) {
			return FINAL_MOVE;
		} else {
			std::cout << "Couldn't get the camera type" << std::endl;
		}
	}

	return NONE;
}

void JsonCommand::setupNewTrCameraEffect() {
	if (m_cameraType != NONE) {
		m_camera.clear();
		for (double cV : currentProcess.get()["cam"]) {
			if (cV >= 1) {
				if (cV == 8) {
					m_camera.push_back(1);
				} else if ((m_cameraType == TR_MANUAL && m_camera.size() < 4) ||
					(m_cameraType == STATIC_MANUAL && m_camera.size() < 2)) {
					if ( (m_cameraType == TR_MANUAL && m_camera.size() < 2) ||
						(m_cameraType == STATIC_MANUAL && m_camera.size() < 1)) {
						m_camera.push_back(((cV+0.5)*CASE_SIZE)/(CASE_ALL));
					} else {
						m_camera.push_back(((cV+0.5)*CASE_SIZE)/(CASE_ALL));
					}
				} else {
					m_camera.push_back(cV);
				}
			} else {
				m_camera.push_back(cV);
			}
		}

		if (currentProcess.get().contains("Voice")) {
			m_effect.reset(new Effect::Voice {currentProcess.get()["Voice"], 0} );
			m_fileSound = DEFAULT_PATH_E + currentProcess.get()["Voice"].get<std::string>() + ".wav";

		} else if(currentProcess.get().contains("GreenBackground")) {
			const json &v = currentProcess.get()["GreenBackground"];
			m_effect.reset(new Effect::GreenBackground{v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]});
			m_fileSound = DEFAULT_PATH_S + v[0].get<std::string>().substr(0, v[0].get<std::string>().find(".")) + ".wav";

		} else {
			m_effect.reset(new Effect::Main());
			m_typeSound = -1;
			m_fileSound = "";
		}

		if (currentProcess.get().contains("sound")) {
			if (currentProcess.get()["sound"][0] == "any") {
				m_typeSound = -1;
			} else if (currentProcess.get()["sound"][0] == "Effect") {
				m_typeSound = 0;
			} else {
				m_fileSound = DEFAULT_PATH_P + currentProcess.get()["sound"][0].get<std::string>();
				m_typeSound = 0;
				std::cout << "The file is: " << m_fileSound << std::endl;
			}

			if (currentProcess.get()["sound"].size() == 3) {
				m_timeSound = currentProcess.get()["sound"][2];
			} else {
				m_timeSound = 0;
			}

			m_volumeSound = currentProcess.get()["sound"][1];
		} else {
			m_typeSound = -1;
			m_timeSound = 1.2;
		}
	}
}

float JsonCommand::getCurrentRatio(float index, float timeEffect) {
	if (m_cameraType == NONE) {
		std::cerr << "ERROR: Unknown camera type" << std::endl;
		return -1;
	}

	if (m_cameraType == STATIC_MANUAL) {
		return m_camera[2];
	} else if (m_cameraType == FINAL_MOVE) {
		return m_camera[1];
	}

	return m_camera[4] + (m_camera[5] - m_camera[4])*(index/timeEffect);
}

RectEffect JsonCommand::getCamera(const cv::Size &imgSize, const std::size_t currentFrame, const ChessMove &cmove, const std::size_t timeFrame) {
	RectEffect rect {0, 0, 0, 0};

	if (m_cameraType == NONE) {//m_cameraType != TR_MANUAL && m_cameraType != STATIC_MANUAL) {
		std::cerr << "ERROR: Unknown camera type" << std::endl;
		return rect;
	}


	if (m_cameraType == STATIC_MANUAL) {
		rect.height = std::round(static_cast<double>(CASE_ALL)*m_camera[2]);
		rect.width = std::round(static_cast<double>(rect.height*imgSize.width/imgSize.height));//*m_camera[2]);

		const std::size_t vx { START_POINT_X + std::round(static_cast<double>(CASE_ALL)*m_camera[0]) }; 
		const std::size_t vy { START_POINT_Y + std::round(static_cast<double>(CASE_ALL)*m_camera[1]) }; 

		if (vx < rect.width/2) {
			rect.x = 0;
		} else {
			rect.x = vx - rect.width/2;
		}

		if (vy < rect.height/1) {
			rect.y = 0;
		} else {
			rect.y = vy - rect.height/2;
		}

		//std::cout << rect.x << " " << rect.y << " " << rect.width << " " << rect.height << "\n";
	} else if (m_cameraType == TR_MANUAL) {

		double ratioFrame {static_cast<double>(currentFrame)/static_cast<double>(timeFrame)};

		double ratioX { m_camera[0] + (m_camera[1] - m_camera[0])*ratioFrame };
		double ratioY { m_camera[2] + (m_camera[3] - m_camera[2])*ratioFrame };

		double ratioSize { m_camera[4] + (m_camera[5] - m_camera[4])*ratioFrame };

		//rect.width = std::round(static_cast<double>(CASE_ALL)*ratioSize);
		rect.height = std::round(static_cast<double>(CASE_ALL)*ratioSize);
		rect.width = std::round(static_cast<double>(rect.height*imgSize.width/imgSize.height));//*m_camera[2]);

		const std::size_t vx { START_POINT_X + std::round(static_cast<double>(CASE_ALL)*ratioX) }; 
		const std::size_t vy { START_POINT_Y + std::round(static_cast<double>(CASE_ALL)*ratioY) }; 

		if (vx < rect.width/2) {
			rect.x = 0;
		} else {
			rect.x = vx - rect.width/2;
		}

		if (vy < rect.height/2) {
			rect.y = 0;
		} else {
			rect.y = vy - rect.height/2;
		}
	} else if (m_cameraType == FINAL_MOVE) {
		const std::size_t vx = START_POINT_X + cmove.getCenterX(true);
		const std::size_t vy = START_POINT_Y + cmove.getCenterY();

		//rect.width = std::round(static_cast<double>(CASE_ALL)*m_camera[1]);
		rect.height = std::round(static_cast<double>(CASE_ALL)*m_camera[1]);
		//rect.width = std::round(static_cast<double>(rect.height*imgSize.width/imgSize.height)*m_camera[2]);
		rect.width = std::round(static_cast<double>(rect.height*imgSize.width/imgSize.height));//*m_camera[2]);

		if (vx < rect.width/2) {
			rect.x = 0;
		} else {
			rect.x = vx - rect.width/2;
		}

		if (vy < rect.height/2) {
			rect.y = 0;
		} else {
			rect.y = vy - rect.height/2;
		}
	} else if (m_cameraType == TRACK_MOVE) {
		const float &vx = cmove.currentX + 0.5*CASE_SIZE;
		const float &vy = cmove.currentY + 0.5*CASE_SIZE;

		//rect.width = std::round(static_cast<double>(CASE_ALL)*m_camera[1]);
		rect.height = std::round(static_cast<double>(CASE_ALL)*m_camera[1]);
		rect.width = std::round(static_cast<double>(rect.height*imgSize.width/imgSize.height));//*m_camera[2]);

		if (vx < rect.width/2) {
			rect.x = 0;
		} else {
			rect.x = vx - rect.width/2;
		}

		if (vy < rect.height/2) {
			rect.y = 0;
		} else {
			rect.y = vy - rect.height/2;
		}
	} else if (m_cameraType == ALL_MOVE) {
		if (cmove.newX < 9) {
			if (cmove.newX < cmove.oldX) {
				rect.x = START_POINT_X + cmove.newX*CASE_SIZE;
			} else {
				rect.x = START_POINT_X + cmove.oldX*CASE_SIZE;
			}

			if (cmove.newY > cmove.oldY) {
				rect.y = START_POINT_Y + (7 - cmove.newY)*CASE_SIZE;
			} else {
				rect.y = START_POINT_Y + (7 - cmove.oldY)*CASE_SIZE;
			}

			rect.width  = (std::abs(cmove.oldX - cmove.newX) + 1)*CASE_SIZE;
			rect.height = (std::abs(cmove.oldY - cmove.newY) + 1)*CASE_SIZE;

			//double ratioImg { static_cast<double>(imgSize.width)/static_cast<double>(imgSize.height) };

			if (rect.width < rect.height) {
				double newSizeX = static_cast<double>(rect.height * imgSize.width)/static_cast<double>(imgSize.height);

				if ((newSizeX - rect.width)/2 > rect.x) {
					rect.x = 0;
				} else {
					rect.x -= (newSizeX - rect.width)/2;
				}

				rect.width = newSizeX;
			} else {
				double newSizeY = static_cast<double>(rect.width * imgSize.height)/static_cast<double>(imgSize.width);

				if ((newSizeY - rect.height)/2 > rect.x) {
					rect.y = 0;
				} else {
					rect.y -= (newSizeY - rect.height)/2;
				}

				rect.height = newSizeY;
			}

			//std::cout << rect.width << " " << rect.height << " " << rect.x << " " << rect.y << std::endl;
		}
	}

	if (rect.x + rect.width > imgSize.width) {
		rect.x = imgSize.width - rect.width - 1;
	}

	if (rect.y + rect.height > imgSize.height) {
		rect.y = imgSize.height - rect.height - 1;
	}

	return rect;
}

const std::size_t JsonCommand::getTimeEffect() const { //const std::string &key, const std::size_t &index) const {
	if (m_cameraType != NONE) {
		if (currentProcess.get().contains("time")) {
			if (currentProcess.get()["time"] == "Effect") {
				return m_effect->getTimeEffect(frameRate)*frameRate;
			} else {
				return currentProcess.get()["time"].get<float>();
			}
		} else {
			return defaultTime;
		}
	} 
	
	return 0;
}

bool JsonCommand::setEffect(cv::Mat &effect, float fps, float numberFPS,
		double centerX, double centerY, const ChessMove &cmove, double currentRatio,
		const std::size_t currentIndex, const std::size_t type) {

	if (m_cameraType != NONE) {
		RectEffect rect;
		m_effect->setEffect(effect, fps, numberFPS, cmove, rect);

	}

	if (m_timeSound != 1.5 && m_typeSound != -1 && m_fileSound != "") {//fps >= numberFPS*m_timeSound && m_fileSound != "" && m_typeSound != -1) {
		//RenderImage::addAudio(m_fileSound, m_volumeSound);
		m_timeSound = 1.5;
		m_typeSound = -1;
		
		return true;
	}

	return false;
}

std::pair<std::string, float> JsonCommand::audioInformation() {
	return std::make_pair(m_fileSound, m_volumeSound);
}

const std::string JsonCommand::getKeyIndex(std::size_t index, std::size_t indexAction) {
	if (indexAction == 0) {
		return std::to_string(index) + "start";
	} else if (indexAction == 1) {
		return std::to_string(index) + "middle";
	} else if (indexAction == 2) {
		return std::to_string(index) + "end";
	}

	return "";
}

std::pair<double, double> JsonCommand::timeInformation() {
		if(currentProcess.get().contains("GreenBackground")) {
			const json &v = currentProcess.get()["GreenBackground"];
			return std::make_pair(v[5], v[6]);

		}

		return std::make_pair(0, -1);
}


const bool JsonCommand::getCameraBeforeSetup() const { //const std::string &key, const std::size_t &index) const {
	return currentProcess.get()["fcam"];
}

void JsonCommand::setTransitionIDE(const nlohmann::json &cmd, std::size_t frameRate, std::size_t defaultTime) {
	currentProcess = std::cref(cmd);
	this->frameRate = frameRate;
	this->defaultTime = defaultTime;

	m_cameraType = getCameraTypeSetup();//key, index);
	setupNewTrCameraEffect();
}
