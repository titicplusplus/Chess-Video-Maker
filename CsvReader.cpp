#include "CsvReader.hpp"
#include <fstream>
#include <cassert>

CsvReader::CsvReader() {}
CsvReader::~CsvReader() {}

bool CsvReader::openCSVFile(const std::string file) {
	std::ifstream ifs(file);

	if (ifs) {
		std::string line;

		std::size_t index = 0;
		while (std::getline(ifs, line)) {
			std::string::size_type pre_pos {0}, pos{0};
			values.push_back({});

			while ((pos = line.find(',', pos)) != std::string::npos) {
				values[index].push_back(line.substr(pre_pos, pos));
				pre_pos = ++pos;
			}

			index++;
		}
	} else {
		return false;
	}

	ifs.close();

	return true;
}

std::size_t CsvReader::size() const {
	return values.size();
}

std::vector<std::string> &CsvReader::operator[](std::size_t index) {
	assert(index < values.size());
	return values[index];
}
