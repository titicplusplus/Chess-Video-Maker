#include "StockFishUCI.hpp"
#include <unistd.h>
#include <iostream>

#define LOG() std::cout << __LINE__ << " in " << __FUNCTION__ << '\n';


void StockFishInfo::draw() const {
	std::cout << "depth: " << depth
		  << ", seldepth: " << seldepth 
		  << ", multipv: " << multipv 
	  	  << ", cp: " << cp 
		  << ", mate: " << mate
		  << ", wdl: [" << wdl[0] << ", " << wdl[1] << ", " << wdl[2] << ']';

	if (pv.size() != 0)
		std::cout << ", bestmove: " << pv;
	std::cout << '\n';
}



StockFishUCI::StockFishUCI(const char* executable, std::string_view FEN) {
	m_uci.start(executable);
	m_executable = executable;
	
	
	if (FEN != "") {
		isReady();
		positionFEN(FEN);
	}

	isReady();
	//setOption("MultiPV", "3");
}

void StockFishUCI::setOption(const std::string id, const std::string value) {
	m_uci.write( (std::string("setoption name ") + id + " value " + value + "\n").c_str());
	//std::cout << (std::string("setoption name ") + id + " value " + value + "\n");
	//std::cout << "setoption name MultiPV value 3\n";

}

void StockFishUCI::positionFEN(std::string_view FEN) {
	m_uci.write( (std::string("position fen ") + std::string(FEN) + "\n").c_str());
	isReady();
}

void StockFishUCI::isReady() {
	m_uci.write("isready\n");

	do {
		m_uci.waitForReadyRead();
	} while (m_uci.readAll().toStdString().find("readyok") == std::string::npos);

}

std::string StockFishUCI::go(int depth) {
	isReady();

	std::string cmd {""};
	std::string line {""};

	m_uci.waitForBytesWritten();
	m_uci.write( ("go depth " + std::to_string(depth) + "\n").c_str() );


	do {
		m_uci.waitForReadyRead();
		line = m_uci.readAllStandardOutput().toStdString();
		if (line != "") {
			cmd += line;
		} else {
			continue;
		}
	} while (line.find("bestmove") == std::string::npos);

	std::size_t wdl {};

	return cmd;
}

std::tuple<int, int, int> StockFishUCI::getWdlStats(int depth) {
	std::string oldline {""};
	std::string line {""};

	m_uci.waitForBytesWritten();
	m_uci.write( ("go depth " + std::to_string(depth) + "\n").c_str() );

	do {
		oldline = line;

		m_uci.waitForReadyRead();
		line = m_uci.readAllStandardOutput().toStdString();
	} while (line.find("bestmove") == std::string::npos);

	auto indexWdl = oldline.find("wdl") + 4;

	std::array<int, 3> wdlStats {0, 0, 0};

	for (int &fWdl : wdlStats) {
		while (oldline[indexWdl] != ' ') {
			fWdl = fWdl*10 + (oldline[indexWdl] - 48);
			indexWdl++;
		}
		indexWdl++;
	}

	return std::make_tuple(wdlStats[0], wdlStats[1], wdlStats[2]);
}

std::string StockFishUCI::bestMove(int depth) {
	std::string line {""};
	std::size_t index;

	m_uci.waitForBytesWritten();
	m_uci.write( ("go depth " + std::to_string(depth) + "\n").c_str() );

	do {
		m_uci.waitForReadyRead();
		line = m_uci.readAllStandardOutput().toStdString();
		index = line.find("bestmove");
	} while (index == std::string::npos);

	return line.substr(index+9, 4);
}

StockFishInfo StockFishUCI::processLine(std::string line) {
	std::vector<std::string> words{};

    	size_t pos;
    	while ((pos = line.find(" ")) != std::string::npos) {
        	words.push_back(line.substr(0, pos));
        	line.erase(0, pos + 1);
    	}
        	
	words.push_back(line);

	StockFishInfo info;

	for (std::size_t i{0}; i < words.size();) {
		if (words[i] == "depth") {
			info.depth = std::stoi(words[i+1]);
			i += 2;
		} else if (words[i] == "seldepth") {
			info.seldepth = std::stoi(words[i+1]);
			i += 2;
		} else if (words[i] == "multipv") {
			info.multipv = std::stoi(words[i+1]);
			i += 2;
		}  else if (words[i] == "cp") {
			info.cp = std::stoi(words[i+1]);
			i += 2;
		} else if (words[i] == "wdl") {
			info.wdl = {std::stoi(words[i+1]), std::stoi(words[i+2]), std::stoi(words[i+3])};
			i += 4;
		} else if (words[i] == "mate") {
			info.mate = std::stoi(words[i+1]);
			i += 2;
		} else if (words[i] == "pv") {
			//info.pv.insert(info.pv.begin(), words.begin()+i+1, words.end());
			info.pv = words[i+1];
			break;
		} else {
			i++;
		}
	}

	return info;
}

StockFishInfo StockFishUCI::information(int depth) {
	isReady();
	std::string oldline {""};
	std::string line {""};

	m_uci.waitForBytesWritten();
	m_uci.write( ("go depth " + std::to_string(depth) + "\n").c_str() );

	do {
		oldline = line;

		m_uci.waitForReadyRead();
		line = m_uci.readAllStandardOutput().toStdString();
	} while (line.find("bestmove") == std::string::npos);

	return processLine(oldline);
}

std::vector<StockFishInfo> StockFishUCI::allInformation(int depth, std::string fen) {
	isReady();

	std::vector<StockFishInfo> allInfo {};

	std::string line {""};

	m_uci.waitForBytesWritten();
	m_uci.write( ("go movetime 500 depth " + std::to_string(depth) + "\n").c_str() );

	std:size_t indexPP;


	do {
		if (fen != "") {
			positionFEN(fen);
		}

		indexPP = 0;

		while (indexPP < 110) {
			indexPP++;
			m_uci.waitForReadyRead();
			line = m_uci.readAllStandardOutput().toStdString();

			if (line.size() != 0) {
				if (line.find("bestmove") != std::string::npos) {
					break;
				} else if (line.find("depth") != std::string::npos) {
					size_t pos;
					
					while ((pos = line.find("\ninfo")) != std::string::npos) {

						allInfo.push_back(processLine(line.substr(0, pos)));
						line.erase(0, pos + 5);
					}
					
					allInfo.push_back(processLine(line));
				}
			}
		}

		if (indexPP == 110) {
			m_uci.kill();
			m_uci.start(m_executable.c_str());
			isReady();

			std::cout << "ERROR !!!" << std::endl;
			break;
		}
	} while (indexPP == 110);

	return allInfo;
}



StockFishUCI::~StockFishUCI() {
	m_uci.write("quit\n");
	m_uci.waitForFinished();
}
