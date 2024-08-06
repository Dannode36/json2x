#include "CLArgs.h"

#define FMT_HEADER_ONLY
#include "fmt/format.h"

namespace CLOptions {
	bool o_force = false; //"-f": Force parser to output code regardless of possibly fatal errors
	bool o_noErrors = false;//"-ne": hrow an error if any errors are present post-parsing (e.g. placeholder, typeError)
	bool o_logging = false; //"-l": Some extra logging generally for debugging
	std::string o_indent = "    "; //"-i <type> <num>": The indent used for code generation

	void parse(std::vector<std::string>& args)
	{
		o_force = false;
		o_noErrors = false;
		o_logging = false;
		o_indent = "    ";

		for (size_t i = 0; i < args.size(); i++)
		{
			if (args[i] == "-f") {
				o_force = true;
			}
			else if (args[i] == "-i") {
				i++;

				//Quick check to see if an indent multiplier was supplied
				if (i + 1 >= args.size()) {
					throw std::exception(fmt::format("Indent option format is \"-i <type> <num>\"", args[i]).c_str());
				}

				if (args[i] == "tab") {
					o_indent = std::string(std::stoi(args[++i]), '\t');
				}
				else if (args[i] == "space"){
					o_indent = std::string(std::stoi(args[++i]), ' ');
				}
				else {
					throw std::exception(fmt::format("Indent identifier \"{}\" is not valid", args[i]).c_str());
				}
			}
			else if (args[i] == "-l") {

			}
			else if (args[i] == "-ne") {
				o_noErrors = true;
			}

			//State errors
			if (o_noErrors && o_force) { throw std::exception("-f and -ne cannot be set simultaneously"); }
		}
	}

	bool isForced()
	{
		return o_force;
	}
	bool isNoErrors()
	{
		return o_noErrors;
	}
	bool isLogging()
	{
		return o_logging;
	}
	std::string indent()
	{
		return o_indent;
	}
}
