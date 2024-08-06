#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "jsonConverters.h"
#include "win32.h"
#include <csignal>
#include "CLArgs.h"

#define VERSION_MAJOR 2
#define VERSION_MINOR 1

void signal_callback(int signum) {
    std::cout << "Program terminated (" << signum << ")\n";
    exit(signum);
}

int main(int argc, char* argv[]) {
    const std::string className = "MyClass";
    const std::wstring outFileName = L"\\output";
    bool running = true;

    LangFormat format;
    std::string filePath;
    std::string language;

    signal(SIGINT, signal_callback);

    std::cout << "json2x " << VERSION_MAJOR << "." << VERSION_MINOR << "\n";

    while (running)
    {
        //Parse argument input
        {
            filePath.clear();
            language.clear();
            std::vector<std::string> optionalArgs{};

            try
            {
                if (argc > 1) {

                    if (argc < 3) {
                        throw std::exception("Correct syntax is \"json2x <file-path> <language> <options>\"\n");
                    }
                    filePath = argv[1];
                    language = argv[2];

                    for (int i = 3; i < argc; i++) {
                        optionalArgs.push_back(argv[i]);
                    }

                    running = false; //Run once
                    argc = 0; //Prevent args being used again
                }
                else {
                    std::cout << ">> ";
                    std::string tempInput;
                    std::getline(std::cin, tempInput); //Text input is actual f!#ked in C++

                    std::stringstream ss(tempInput);
                    std::getline(ss >> std::ws, filePath, ' ');
                    std::getline(ss >> std::ws, language, ' ');

                    //Parse optional args
                    std::string tempArg;
                    while (std::getline(ss >> std::ws, tempArg, ' ')) {
                        optionalArgs.push_back(tempArg);
                    }
                }
            
                if (filePath == "exit") {
                    exit(1);
                }

                if (filePath.empty() || language.empty()) {
                    throw std::exception("Correct syntax is \"json2x <file-path> <language> <options>\"\n");
                }

                CLOptions::parse(optionalArgs);
            }
            catch (const std::exception& e)
            {
                std::cerr << "ERROR: " << e.what() << "\n";
                continue;
            }

            //Check if language format is supported
            if (globalFormats.find(language) == globalFormats.end()) {
                std::cerr << "ERROR: \"" + language + "\" is not a supported language\n";
                continue;
            }
            else {
                format = globalFormats.at(language);
            }
        }


        //Load JSON file
        std::ifstream jsonFile(filePath);

        //Read file contents into a string
        std::string json((std::istreambuf_iterator<char>(jsonFile)),
            std::istreambuf_iterator<char>());

        //Construct a code generator object with an indent style (4 spaces) and an initial class name ("MyClass")
        CodeGenerator generator(CLOptions::indent(), className);

        //Write generated code to a file if no parsing error occured
        std::string genOutput = generator.convertJson(json, format);
        if (generator.getLastError() != GenErrorNone) { //Error occured during JSON conversion/code generation

            if (generator.getLastError() != GenErrorInvalidJson) {
                std::cerr << "An error occured";
            }
            continue;
        }
        else {
            std::ofstream outFile(getWorkingDirectory() + outFileName + format.file_extension);
            outFile << genOutput;
            std::cout << "Code generation successfull. Saved result to: ";
            std::wcout << getWorkingDirectory() + outFileName << format.file_extension << "\n";
        }
    }
}
