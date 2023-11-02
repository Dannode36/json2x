#include <iostream>
#include <fstream>
#include "rapidjson/fwd.h"
#include "rapidjson/error/en.h"
#include "jsonConverters.h"
#include "win32Dialogs.h"

int main(int argc, char* argv[]) {
    //Using a windows dialog, get the path to a file for JSON document parsing
    std::ifstream jsonFile(openFileDialog());

    //Read file contents into a string
    std::string json((std::istreambuf_iterator<char>(jsonFile)),
        std::istreambuf_iterator<char>());

    //Construct JSON document and parse
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError()) {
        fprintf(stderr, "\nError(offset %u): %s\n",
            (unsigned)doc.GetErrorOffset(),
            GetParseError_En(doc.GetParseError()));
        return -1;
    }

    //Construct the code generation class with a specific indent style (4 spaces) used when generating code
    const std::string indent = "    ";
    CppGenerator generator(indent);

    //Write generated c++ code to a header file
    std::ofstream outFile("output.h");
    outFile << generator.json2Cpp(doc);

    return 0;
}
