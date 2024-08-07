#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "jsonConverters.h"
#include "win32.h"
#include <csignal>
#include "Options.h"

constexpr auto VERSION_MAJOR = 2;
constexpr auto VERSION_MINOR = 1;

int main(int argc, char* argv[]) {
    const std::string className = "MyClass";
    const std::wstring outFileName = L"\\output";
    bool running = true;

    LangFormat format;
    std::string filePath;
    std::string language;

    std::cout << "json2x " << VERSION_MAJOR << "." << VERSION_MINOR << "\n";

    while (running)
    {
        try
        {
            //Parse argument input
            {
                filePath.clear();
                language.clear();
                std::vector<std::string> optionalArgs{};

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
                    throw std::exception("Correct syntax is \"<file-path> <language> <options>\"");
                }

                //Check if language format is supported
                if (globalFormats.find(language) == globalFormats.end()) {
                    format.parseFormatByName(language);
                }
                else {
                    format = globalFormats.at(language);
                }

                CLOptions::parse(optionalArgs);
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

            //Write generated code to file
            auto dir = CLOptions::outputDirectory().empty() ? getWorkingDirectory() : CLOptions::outputDirectory();
            std::wstring wsFileExtension = std::wstring(format.file_extension.begin(), format.file_extension.end());
            std::ofstream outFile(dir + outFileName + L'.' + wsFileExtension);
            outFile << genOutput;

            std::cout << "Code generation successfull. Saved result to: ";
            std::wcout << dir + outFileName << wsFileExtension << "\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: " << e.what() << "\n";
            continue;
#ifndef NDEBUG
            throw e; //Debug builds will rethrow the exception
#endif
        }
    }
}
