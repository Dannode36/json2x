#include <fstream>
#include <filesystem>
#include <iostream>
#include "jsonConverters.h"
#include "win32.h"

int main(int argc, char* argv[]) {
    const std::string className = "MyClass";
    const std::string indent = "    ";
    const std::wstring outFileName = L"\\output";

    std::string language("csharp");
    LangFormat format;
    //Check if language format is supported
    if (globalFormats.find(language) == globalFormats.end()) {
        std::cout << "ERROR: \"" + language + "\" is not a supported language\n";
        return -1;
    }
    else {
        format = globalFormats.at(language);
    }

    //Using a windows dialog, get the path to a file for JSON document parsing
    std::ifstream jsonFile(openFileDialog());

    //Read file contents into a string
    std::string json((std::istreambuf_iterator<char>(jsonFile)),
        std::istreambuf_iterator<char>());

    //Construct the code generation class with a specific indent style (4 spaces) used when generating code
    CodeGenerator generator(indent, className);

    //Write generated c++ code to a header file if no parsing error occured
    std::string codeText = generator.convertJson(json, format);
    if (codeText.empty()) { //Error occured during JSON conversion/code generation
        return -1; 
    }
    else {
        std::ofstream outFile(getWorkingDirectory() + outFileName + format.file_extension);
        outFile << codeText;
        std::cout << "Code generation successfull. Saved result to: ";
        std::wcout << getWorkingDirectory() + outFileName << format.file_extension << "\n";
        return 0;
    }
}
