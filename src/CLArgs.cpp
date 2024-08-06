#include "CLArgs.h"
#include <string>

#define FMT_HEADER_ONLY
#include "fmt/format.h""

namespace CLOptions {
	bool o_force = false;
	std::string o_indent;

	void parse(int argc, char* argv[])
	{
		for (size_t i = 0; i < argc; i++)
		{
			if (argv[i] == "-f") {
				o_force = true;
			}
			else if (argv[i] == "-i") {
				i++;
				if (argv[i] == "tab") {
					o_indent = std::string(std::stoi(argv[++i]), '\t');
				}
				else if (argv[i] == "space"){
					o_indent = std::string(std::stoi(argv[++i]), ' ');
				}
				else {
					throw std::exception(fmt::format("Indent identifier \"{}\" is not valid", argv[i]).c_str());
				}
			}
			else if (argv[i] == "") {

			}
		}
	}
}
