#include "Options.h"
#include "win32.h"
#include <codecvt>
#include <locale>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

constexpr auto FILE_DIALOG_CMD = "-p";		//Allows the user to select file paths using a file dialog
constexpr auto FOLDER_DIALOG_CMD = "-dp";	//Allows the user to select directory paths using a file dialog

namespace CLOptions {
	bool o_pushThroughErrors = false;		//"-f": Force json2x to output code regardless of possibly fatal errors
	bool o_forcePerfection = false;			//"-p": Throw an error if any placeholder are present after code generation
	bool o_logging = false;					//"-l": Some extra logging generally for debugging
	std::string o_indent = "    ";			//"-i <type> <num>": The indent used for code generation
	std::wstring outputDir = L"";				//"-o <directory>"Custom output directory. Leave blank for default (working directory)

	void parse(std::vector<std::string>& args)
	{
		o_pushThroughErrors = false;
		o_forcePerfection = false;
		o_logging = false;
		o_indent = "    ";
		outputDir = L"";

		//Begin by first replacing fileDialog commands with their chosen paths
		/*using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;
		for (size_t i = 0; i < args.size(); i++)
		{
			if (args[i] == FILE_DIALOG_CMD) {
				args[i] = converter.to_bytes(openFileDialog());
			}
		}*/ //This might be too hard (Probably need to use IFileDialog (that shit is wack))

		for (size_t i = 0; i < args.size(); i++)
		{
			if (args[i] == "-f") {
				o_pushThroughErrors = true;
			}
			else if (args[i] == "-p") {
				o_forcePerfection = true;
			}
			else if (args[i] == "-l") {
				o_logging = true;
			}
			else if (args[i] == "-i") {
				i++;

				//Quick check to see if all args were was supplied
				if (i + 1 >= args.size()) {
					throw std::exception("The correct syntax is \"-i <type> <num>\"");
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
			else if (args[i] == "-o") {

				if (!outputDir.empty()) {
					throw std::exception("-o cannot be set more than once");
				}

				if (i + 1 >= args.size() || args[i + 1][0] == '-') {
					throw std::exception("The correct syntax is \"-o <directory>\"");
				}

				i++;
				outputDir = std::wstring(args[i].begin(), args[i].end());
			}
			else {
				throw std::exception(fmt::format("Unrecognised parameter \"{}\"", args[i]).c_str());
			}

			//State errors
			if (o_forcePerfection && o_pushThroughErrors) { throw std::exception("-f and -p cannot be set simultaneously"); }
		}
	}

	bool isPushThroughErrors()
	{
		return o_pushThroughErrors;
	}
	bool isForcePerfection()
	{
		return o_forcePerfection;
	}
	bool isLogging()
	{
		return o_logging;
	}
	std::string indent()
	{
		return o_indent;
	}
	std::wstring outputDirectory()
	{
		return outputDir;
	}
}
