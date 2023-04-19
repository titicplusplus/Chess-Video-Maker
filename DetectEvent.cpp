#include "DetectEvent.hpp"

DetectChess::DetectChess() : ChessParser() {}
DetectChess::~DetectChess() {}

std::vector<std::string> DetectChess::triggerEvents() {

	if (indexInstruction == 0 || indexInstruction == instructions.size()+1) {
		std::cout << "Error " << indexInstruction << " is not a valid" << std::endl;
		return {};
	}

	std::string event {instructions[indexInstruction-1]};

	if (event[event.size()-1] == '+') {
	}

	return {};
}

void doubleAttack(std::vector<std::string> trigE, int colorPlayer) {
	//if (
}
