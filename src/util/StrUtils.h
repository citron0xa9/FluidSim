#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace strutils {
	std::vector<std::string> split(const std::string& src, const char delim)
	{
		std::istringstream iss(src);
		std::string elem;
		std::vector<std::string> elements;
		while (!iss.eof()) {
			std::getline(iss, elem,delim);
			elements.push_back(elem);
		}
		return elements;
	}
}