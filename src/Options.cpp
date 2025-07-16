#include "Options.h"
#include "win32.h"
#include <codecvt>
#include <locale>
#include <map>
#include <functional>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

constexpr auto FILE_DIALOG_CMD = "-fs";		//Allows the user to select file paths using a file dialog
constexpr auto FOLDER_DIALOG_CMD = "-fs";	//Allows the user to select directory paths using a file dialog

namespace CLOptions {
	struct Option
	{
		std::string description;
		std::function<void(std::vector<std::string>&, size_t&)> func;
	};

	std::string r_filePath = "";
	LangFormat r_LanguageFormat;

	bool o_forceThroughErrors = false;		//"-f":					Force json2x to output code regardless of possibly fatal errors
	bool o_forcePerfection = false;			//"-p":					Throw an error if any placeholder are present after code generation
	bool o_logging = false;					//"-l":					Some extra logging generally for debugging
	std::string o_indent = "    ";			//"-i <type> <num>":	The indent used for code generation
	std::wstring outputDir = L"";			//"-o <directory>"		Custom output directory. Leave blank for default (working directory)

	std::map<std::string, Option> commandStringToId
	{
		{ "-f", { "-f: Force json2x to output code regardless of possibly fatal errors", 
		[](std::vector<std::string>& args, size_t& i) {
			o_forceThroughErrors = true;
		}}},

		{ "-p", { "-p: Throw an error if placeholders are present after code generation.Requires the output to be perfect", 
		[](std::vector<std::string>& args, size_t& i) {
			o_forcePerfection = true;
		}}},

		{ "-l", { "-l: Some extra logging for debugging and nerds", 
		[](std::vector<std::string>& args, size_t& i) {
			o_logging = true;
		}}},

		{ "-i", { "-i <type> <num>: The indent used for code generation(currently only supports \"space\" and \"tab\")",
		[](std::vector<std::string>& args, size_t& i) {
			i++;

			//Quick check to see if all args were was supplied
			if (i + 1 >= args.size()) {
				throw std::exception("The correct syntax is \"-i <type> <num>\"");
			}

			if (args[i] == "tab") {
				o_indent = std::string(std::stoi(args[++i]), '\t');
			}
			else if (args[i] == "space") {
				o_indent = std::string(std::stoi(args[++i]), ' ');
			}
			else {
				throw std::exception(fmt::format("Indent identifier \"{}\" is not valid", args[i]).c_str());
			}
		}}},

		{ "-o", { "-o: <directory>: Custom output directory.The default is the working directory",
		[](std::vector<std::string>& args, size_t& i) {
			if (!outputDir.empty()) {
				throw std::exception("-o cannot be set more than once");
			}

			if (i + 1 >= args.size() || args[i + 1][0] == '-') {
				throw std::exception("The correct syntax is \"-o <directory>\"");
			}

			i++;
			outputDir = std::wstring(args[i].begin(), args[i].end());
		}}}
	};

	//Returns: Wether the current command should continue execution. e.g. "help" will return false
	bool parse(std::vector<std::string>& args)
	{
		if (args.size() > 0 && args[0] == "help") {
			CLOptions::help();
			return false;
		}

		if (args.size() < 2) {
			throw std::exception("Correct syntax is \"<file-path> <language> <options>\"");
		}

		//Begin by first replacing fileDialog commands with their chosen paths
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;
		for (size_t i = 0; i < args.size(); i++)
		{
			if (args[i] == FILE_DIALOG_CMD) {
				args[i] = converter.to_bytes(openFileDialog());
			}
		} //This might be too hard (Probably need to use IFileDialog (that shit is wack))

		//Get filepath
		r_filePath = args[0];

		//Check if language format is supported
		auto formatIter = globalFormats.find(args[1]);
		if (formatIter == globalFormats.end()) {
			r_LanguageFormat.parseFormatByName(args[1]);
		}
		else {
			r_LanguageFormat = formatIter->second;
		}

		//Reset optional state
		o_forceThroughErrors = false;
		o_forcePerfection = false;
		o_logging = false;
		o_indent = "    ";
		outputDir = L"";

		for (size_t i = 2; i < args.size(); i++)
		{
			auto iter = commandStringToId.find(args[i]);
			if (iter != commandStringToId.end()) {
				//This function can modify the loops iterator when options have their own args
				iter->second.func(args, i);
			}
			else {
				throw std::exception(fmt::format("Unrecognised parameter \"{}\"", args[i]).c_str());
			}

			//State errors
			if (o_forcePerfection && o_forceThroughErrors) { throw std::exception("-f and -p cannot be set simultaneously"); }
		}

		return true;
	}

	std::string filePath()
	{
		return r_filePath;
	}

	LangFormat& langFormat()
	{
		return r_LanguageFormat;
	}

	bool isPushThroughErrors()
	{
		return o_forceThroughErrors;
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
	void help(std::string arg)
	{
		fmt::print("Syntax: json2x <file-path> <language> <options>\n");

		for (auto& option : commandStringToId)
		{
			fmt::print(option.second.description + '\n');
		}
	}
}
