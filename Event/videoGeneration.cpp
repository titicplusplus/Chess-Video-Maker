#include "videoGeneration.hpp"
#include <vector>

#ifndef __has_include
  static_assert(false, "__has_include not supported");
#else
#  if __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
     namespace fs = std::experimental::filesystem;
#  elif __has_include(<filesystem>)
#    include <filesystem>
     namespace fs = std::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp>
     namespace fs = boost::filesystem;
#  endif
#endif



namespace TitiCplusplus {

nlohmann::json VideoGeneration::getResVideo(int defaultType, const std::vector<ChessMove> &cmove, std::size_t m_currentIndex) {
	std::vector<int> videos {res.solution(defaultType)};

	if (videos.size() == 0) {
		std::cerr << "Can't find video" << '\n';
		return {};
	}

	std::size_t videoIndex {videos[Default::randomTime(0, videos.size())]};
	nlohmann::json videoInfo = res.getInfoVideo(videoIndex);

	//std::cout << videoInfo.dump(4) << '\n';

	while (!videoInfo["green"].get<bool>()) {
		videoIndex = videos[Default::randomTime(0, videos.size())];
		videoInfo = res.getInfoVideo(videoIndex);
	}

	std::string file {std::to_string(videoIndex)};

	if (!fs::exists("Ressources/video/" + file + ".mp4")) {
		if (!fs::exists("Ressources/video/" + file + ".mkv")) {
			file += ".webm";
		} else {
			file += ".mkv";
		}
	} else {
		file += ".mp4";
	}

	float m_timeStart {0};
	float m_timeEnd {0};

	if (videoInfo["time"] == "any") {
		m_timeEnd = 0.5;
	} else if (videoInfo["time"] == "all") {
		m_timeEnd = -1;
	} else if (videoInfo["time"].type() == nlohmann::json::array()) {
		std::size_t timeIndex {Default::randomTime(0, videoInfo["time"].size())};

		m_timeStart = videoInfo["time"][timeIndex][0];
		m_timeEnd = videoInfo["time"][timeIndex][0];
	} else if (videoInfo["time"].type() == nlohmann::json::object()) {
		float tstart {0};
		float tstartMin {0};
		float tend {0};

		do {
			if (videoInfo["time"].contains("min")) {
				tstart  = videoInfo["time"]["min"];
			} 

			if (videoInfo["time"].contains("dmin")) {
				tstartMin = videoInfo["time"]["dmin"];
			} else {
				tstartMin = tstart;
			}	

			if (videoInfo["time"].contains("max")) {
				tend = videoInfo["time"]["max"];
			} else {
				cv::VideoCapture video { "Ressources/video/" + file };
				tend = video.get(cv::CAP_PROP_FRAME_COUNT)/video.get(cv::CAP_PROP_FPS);
			}

			m_timeStart = tstart;
			m_timeEnd = Default::randomTime(tstartMin, tend);
			std::cout << file << ":" << tstart << " " << tstartMin << " " << tend << " " << tend - tstart << " " << (tend - tstart < 0.2)<< "\n";
		} while (tend - tstart < 0.2);
	}

	if (defaultType == Ressources::BadMove || defaultType == Ressources::GoodMove ||
			defaultType == Ressources::BugPlayer) {

		return {std::to_string(m_currentIndex) + "end", {{
				{"fcam", true},
				{"cam", {cmove[m_currentIndex].newX, 7 - cmove[m_currentIndex].newY, 0.5}},
				{"time", "Effect"},
				{"sound", {"Effect", 0.5}},
				{"GreenBackground", {file, -1, -1, 0.5, 0.5, m_timeStart, m_timeEnd, 0}},
			}}
		};
	}

	return {std::to_string(m_currentIndex) + "middle", {{
		//nlohmann::json::array({
				{"fcam", true},
				{"cam", {2, 0.5}},
				{"time", "Effect"},
				{"sound", {"Effect", 0.5}},
				{"GreenBackground", {file, -1, -1, 0.5, 0.5, m_timeStart, m_timeEnd, 0}},
		//})
		}}
	};

}

}; // TitiCplusplus
