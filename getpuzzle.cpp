#include "getpuzzle.hpp"
#include <sstream>
#include <iostream>

GetPuzzle::GetPuzzle() {}
GetPuzzle::~GetPuzzle() {}

using json = nlohmann::json;

json GetPuzzle::getPuzzle() {
	cURLpp::Cleanup myCleanup;

	cURLpp::Options::Url lichessURL(
			std::string("https://lichess.org/api/puzzle/daily"));

	cURLpp::Options::Port lichessPORT(443);
	cURLpp::Easy myRequest;

	myRequest.setOpt(lichessURL);
	myRequest.setOpt(lichessPORT);

	std::stringstream result;
	myRequest.setOpt(cURLpp::Options::WriteStream(&result));

	myRequest.perform();

	//result << myRequest;

	json data = json::parse(result.str());//.substr(0, int(result.str().size()/2)));

	return data;
}


