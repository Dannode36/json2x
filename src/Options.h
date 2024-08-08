#pragma once
#include <string>
#include <vector>
#include "LangFormat.h"

namespace CLOptions {
	void parse(std::vector<std::string>& args);

	std::string filePath();
	LangFormat& langFormat();

	bool isPushThroughErrors();
	bool isForcePerfection();
	bool isLogging();
	std::string indent();
	std::wstring outputDirectory();

	void help(std::string arg = "");
}
