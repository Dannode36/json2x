#include <iostream>
#include <fstream>
#include "rapidjson/fwd.h"
#include "rapidjson/error/en.h"
#include "jsonConverters.h"
#include "win32Dialogs.h"

static int i = 0;

int main(int argc, char* argv[]) {
    const std::string indent = "    ";

    std::ifstream jsonFile(openFileDialog());
    std::string json((std::istreambuf_iterator<char>(jsonFile)),
        std::istreambuf_iterator<char>());

    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError()) {
        fprintf(stderr, "\nError(offset %u): %s\n",
            (unsigned)doc.GetErrorOffset(),
            GetParseError_En(doc.GetParseError()));
        return -1;
    }

    CppGenerator generator;
    std::cout << generator.json2Cpp(doc, indent);

    return 0;
}
