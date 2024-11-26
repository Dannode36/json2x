#include <fstream>
#include "LangFormat.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "fmt/format.h"

void LangFormat::parseFormatByName(std::string name)
{
    //Load JSON file
    std::ifstream formatFile(USER_FORMATS_PATH + name + ".json");

    if (!formatFile.is_open()) {
        throw std::exception(fmt::format("\"{}\" could not be found in {}\n", name, USER_FORMATS_PATH).c_str());
    }

    //Read file contents into a string
    std::string json((std::istreambuf_iterator<char>(formatFile)),
        std::istreambuf_iterator<char>());

    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError()) {
        throw std::exception(
            fmt::format("ERROR: User format file: (offset {}}): {}\n",
                (unsigned)doc.GetErrorOffset(),
                GetParseError_En(doc.GetParseError())).c_str()
        );
    }

    this->int32_t = doc["int32"].GetString();
    this->uint32_t = doc["uint32"].GetString();
    this->int64_t = doc["int64"].GetString();
    this->uint64_t = doc["uint64"].GetString();
    this->float_t = doc["float32"].GetString();
    this->double_t = doc["float64"].GetString();
    this->bool_t = doc["bool"].GetString();
    this->string_t = doc["string"].GetString();
    this->placeholder_t = doc["placeholder"].GetString();
    this->var_format = doc["var"].GetString();
    this->array_format = doc["array"].GetString();
    this->structS_format = doc["structStart"].GetString();
    this->structE_format = doc["structEnd"].GetString();
    this->file_extension = doc["extension"].GetString();

    auto usings = doc["usings"].GetObject();
    for (auto i = usings.MemberBegin(); i != usings.MemberEnd(); i++) {
        this->usings.insert({ i->name.GetString(), i->value.GetString() });
    };
}
