#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "jsonConverters.h"
#include "win32.h"
#include <csignal>

#define VERSION_MAJOR 1
#define VERSION_MINOR 0

void signal_callback(int signum) {
    std::cout << "Program terminated (" << signum << ")\n";
    exit(signum);
}

int main(int argc, char* argv[]) {
    const std::string className = "MyClass";
    const std::string indent = "    ";
    const std::wstring outFileName = L"\\output";
    const bool cli = argc < 2;
    LangFormat format;
    std::string filePath;
    std::string language;

    signal(SIGINT, signal_callback);
    if (cli) {
        std::cout << "json2x " << VERSION_MAJOR << "." << VERSION_MINOR << "\n";
    }

    do {
        if (cli) {
            filePath.clear();
            language.clear();

            std::cout << ">> ";
            std::string tempInput;
            std::getline(std::cin >> std::ws, tempInput); //Text input is actual f!#ked in C++

            std::stringstream ss(tempInput);
            std::getline(ss, filePath, ' ');
            std::getline(ss, language, ' ');

            if (filePath == "exit") {
                exit(1);
            }

            if (filePath.empty() || language.empty()) {
                std::cerr << "ERROR: Correct syntax is \"json2x <file-path> <language>\"\n";
                continue;
            }
        }
        else { //Not in CLI mode
            if (argc < 3) {
                std::cerr << "ERROR: Correct syntax is \"json2x <file-path> <language>\"\n";
                continue;
            }
            else {
                filePath = argv[1];
                language = argv[2];
            }
        }

        //Check if language format is supported
        if (globalFormats.find(language) == globalFormats.end()) {
            std::cerr << "ERROR: \"" + language + "\" is not a supported language\n";
            continue;
        }
        else {
            format = globalFormats.at(language);
        }

        //Load JSON file
        std::ifstream jsonFile(filePath);

        //Read file contents into a string
        std::string json((std::istreambuf_iterator<char>(jsonFile)),
            std::istreambuf_iterator<char>());

        //Construct a code generator object with an indent style (4 spaces) and an initial class name ("MyClass")
        CodeGenerator generator(indent, className);

        //Write generated code to a file if no parsing error occured
        std::string genOutput = generator.convertJson(json, format);
        if (generator.getLastError() != GenErrorNone) { //Error occured during JSON conversion/code generation
            
            if (generator.getLastError() != GenErrorInvalidJson) {
                std::cerr << genOutput;
            }
            continue;
        }
        else {
            std::ofstream outFile(getWorkingDirectory() + outFileName + format.file_extension);
            outFile << genOutput;
            std::cout << "Code generation successfull. Saved result to: ";
            std::wcout << getWorkingDirectory() + outFileName << format.file_extension << "\n";
        }

    } while (cli);
}
