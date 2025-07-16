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
    const std::string className = "Class";
    const std::wstring outFileName = L"\\output";
    bool running = true;

    if (argc <= 1) {
        std::cout << "json2x " << VERSION_MAJOR << "." << VERSION_MINOR << " (shell mode) " << "\n";
    }

    while (running)
    {
        try
        {
            //Parse argument input
            {
                std::vector<std::string> args{};

                if (argc > 1) {

                    if (argc < 3) {
                        throw std::exception("Correct syntax is \"json2x <file-path> <language> <options>\"\n");
                    }

                    for (int i = 1; i < argc; i++) {
                        args.push_back(argv[i]);
                    }

                    running = false; //Run once
                    argc = 0; //Prevent args being used again
                }
                else {
                    std::cout << ">> ";

                    std::string tempInput;
                    std::getline(std::cin, tempInput); //Text input is actual f!#ked in C++
                    std::stringstream userInput(tempInput);

                    //Parse optional args
                    std::string tempArg;
                    while (std::getline(userInput >> std::ws, tempArg, ' ')) {
                        if (tempArg == "exit") {
                            exit(1);
                        }

                        args.push_back(tempArg);
                    }
                }

                if (CLOptions::parse(args) != true) {
                    continue;
                }
            }

            //Load JSON file
            std::ifstream jsonFile(CLOptions::filePath());

            if (!jsonFile.is_open()) {
                throw std::exception(("The file \"" + CLOptions::filePath() + "\" could not be opened").c_str());
            }

            //Read file contents into a string
            std::string json((std::istreambuf_iterator<char>(jsonFile)),
                std::istreambuf_iterator<char>());

            //Construct a code generator object with an indent style (4 spaces) and an initial class name ("MyClass")
            CodeGenerator generator(CLOptions::indent(), className);

            //Get generated code if no parsing error occured
            std::string genOutput = generator.convertJson(json, CLOptions::langFormat());

            //Write generated code to file
            auto dir = CLOptions::outputDirectory().empty() ? getWorkingDirectory() : CLOptions::outputDirectory();
            std::wstring wsFileExtension = std::wstring(CLOptions::langFormat().file_extension.begin(), CLOptions::langFormat().file_extension.end());
            std::ofstream outFile(dir + outFileName + L'.' + wsFileExtension);
            outFile << genOutput;

            std::cout << "Code generation successfull. Saved result to: ";
            std::wcout << dir + outFileName << '.' << wsFileExtension << "\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << "ERROR: " << e.what() << "\n";
#ifdef NDEBUG
            continue;
#else
            throw e; //Debug builds will rethrow the exception
#endif
        }
    }
}
