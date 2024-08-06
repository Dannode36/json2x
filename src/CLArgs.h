#pragma once
#include <string>
#include <vector>

namespace CLOptions {
	void parse(std::vector<std::string>& args);
	bool isForced();
	bool isNoErrors();
	bool isLogging();
	std::string indent();
}

