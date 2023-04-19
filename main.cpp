#include <iostream>
#include "ChessAutoMaker.hpp"
#include <fstream>
//#include "default.hpp"
#include <nlohmann/json.hpp>
#include "ChessRenderImage.hpp"
//#include <opencv2/opencv.hpp>
//#include "OpenMusic.hpp"
#include <unistd.h>
#include <csignal>
#include <QCoreApplication>

//#include "StockFishCom.hpp"
#include "SaveMusic.hpp"

//#include <SFML/Audio.hpp>
#include <unistd.h>
#include "ChessWindow.hpp"

//#include "StockFishUCI.hpp"

#include <ctime>
#include "cmdparser.hpp"


/// https://www.youtube.com/watch?v=HV6h7MRrRNA

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

void openJsonFile(nlohmann::json &jsonFile, const char* filename) {
	if (fs::exists(filename)) {
		std::ifstream file {filename};
		file >> jsonFile;
		file.close();
	} else {
		GetPuzzle puzzle;

		jsonFile["render"] = {{"defaultTime", 30},{"frameRate", 60}};
		jsonFile["data"] = puzzle.getPuzzle();

		//std::ofstream outJ {jfile};
		//outJ << std::setw(4) << allSettings << std::endl;
	}
}



void help() {
	std::cout << "Help:\n";
	std::cout << "\tChessVideoMaker json_file_name" << std::endl;
}

void configure_parser(cli::Parser& parser) {
	parser.set_required<std::string>("d", "data", "The json files");
	parser.set_required<std::string>("t", "type", "what do you want ? => graphics, automaker, render");
	parser.set_optional<std::string>("f", "filename", "outcpp.avi", "the avi file output");
}

int main(int argc, char **argv) {
	cli::Parser parser {argc, argv};
	configure_parser(parser);
	parser.run_and_exit_if_error();

	/**system("ffmpeg -y -i outcpp.avi -i yeah.wav -c copy -shortest -map 0:v:0 -map 1:a:0 final.avi");**/

	std::string type {parser.get<std::string>("t")};

	if (type == "graphics") {
		QApplication app {argc, argv};

		ChessWindow chess {parser.get<std::string>("d")};
		chess.showMaximized();
		chess.show();

		return app.exec();
	} else if (type == "automaker") {
		RenderImage img;

		nlohmann::json data;

		openJsonFile(data, parser.get<std::string>("d").c_str());

		img.startIDE(data["data"]);

		AutoMaker m_auto;
		
		if (m_auto.newModifFile(img.getChessMove(), data)) {
			std::ofstream endfile {parser.get<std::string>("d")};
			endfile << data.dump(1);
			endfile.close();
		}

		std::vector<std::unique_ptr<TitiCplusplus::Event>> events = m_auto.makeAnalyse(img.getChessMove(), data);

		for (const auto &ev : events) {
			std::cout << ev->getCurrentIndex()  << ':' << ev->getRank() << ' ' << ev->getType() << '\n';
		}

		std::cout << "Generation Part\n";

		

		nlohmann::json output = {
			{"frameRate", 60},
			{"defaultTime", 30},
		};

		for (const auto &ev : events) {
			nlohmann::json result {ev->getCmd(img.getChessMove())};

			for (const auto &[key, value] : result.items()) {
				output[key] = value;
			}
		}

		//std::cout << output.dump(4) << '\n';

		data["render"] = output;

		std::ofstream endfile {parser.get<std::string>("d")};
		endfile << data.dump(1);
		endfile.close();
	} else if (type == "render") {
		RenderImage img;
	
		std::ifstream file {parser.get<std::string>("d")};
		nlohmann::json data;

		if (!file) {
			std::cerr << "Can't find " << parser.get<std::string>("d") << '\n';
			return 0;
		}

		file >> data;
		file.close();

		img.startIDE(data["data"]);
		std::vector<std::vector<cv::Mat>> renderVec;
		img.fastRenderAsync(0, img.getChessMove().size()-1, data, renderVec, 
				parser.get<std::string>("f"));
	
		system("ffmpeg -y -i outcpp.avi -i yeah.wav -c copy -shortest -map 0:v:0 -map 1:a:0 final.avi");
	}
}
