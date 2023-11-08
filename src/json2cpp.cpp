#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "jsonConverters.h"
#include "win32.h"

int main(int argc, char* argv[]) {
    const std::string className = "MyClass";
    const std::string indent = "    ";
    const std::wstring outFileName = L"\\output";
    const bool cli = argc < 2;
    LangFormat format;
    std::string filePath;
    std::string language;

    do {
        if (cli) {
            std::cout << ">> ";
            std::cin >> filePath;
            std::cin >> language;

            if (filePath.empty() || language.empty()) {
                std::cerr << ">> ERROR: Incorrect Syntax. Syntax is \"json2x <file-path> <language>\"\n";
            }
        }
        else { //Not in CLI mode
            if (argc < 3) {
                std::cerr << "ERROR: Incorrect Syntax. Syntax is \"json2x <file-path> <language>\"\n";
                continue;
            }
            else {
                filePath = argv[1];
                language = argv[2];
            }
        }

        //Check if language format is supported
        if (globalFormats.find(language) == globalFormats.end()) {
            std::cerr << ">> " << "ERROR: \"" + language + "\" is not a supported language\n";
            continue;
        }
        else {
            format = globalFormats.at(language);
        }

        //Using a windows dialog, get the path to a file for JSON document parsing
        std::ifstream jsonFile(filePath);

        //Read file contents into a string
        std::string json((std::istreambuf_iterator<char>(jsonFile)),
            std::istreambuf_iterator<char>());

        //Construct the code generation class with a specific indent style (4 spaces) used when generating code
        CodeGenerator generator(indent, className);

        //Write generated c++ code to a header file if no parsing error occured
        std::string codeText = generator.convertJson(json, format);
        if (codeText.empty()) { //Error occured during JSON conversion/code generation
            std::cerr << ">> ERROR: An error occured while converting the JSON (convertJson() returned an empty string) :(\n";
            continue;
        }
        else {
            std::ofstream outFile(getWorkingDirectory() + outFileName + format.file_extension);
            outFile << codeText;
            std::cout << ">> " << "Code generation successfull. Saved result to: ";
            std::wcout << getWorkingDirectory() + outFileName << format.file_extension << "\n";
            continue;
        }

    } while (cli);
}
