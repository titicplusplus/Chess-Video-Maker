#pragma once

#include <vector>
#include <string>


class CsvReader {
	public:
		CsvReader();
		~CsvReader();

		bool openCSVFile(const std::string file);
		std::vector<std::string> &operator[](std::size_t index);
		std::size_t size() const;
	
	private:
		std::vector<std::vector<std::string>> values;
};
