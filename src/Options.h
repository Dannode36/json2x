#pragma once
#include <string>
#include <vector>

namespace CLOptions {
	void parse(std::vector<std::string>& args);
	bool isPushThroughErrors();
	bool isForcePerfection();
	bool isLogging();
	std::string indent();
	std::wstring outputDirectory();
}
