#include <iostream>
#include <string>
#include "jsonConverters.h"
#include "rapidjson/error/en.h"
#include <sstream>

#define FMT_HEADER_ONLY
#include "fmt/format.h"

constexpr auto MAX_DEPTH = 512; //Arbitrary number;
std::string typeError = "typeError";

//Special Funcs (public & private) :p
GeneratorErrorCode CodeGenerator::getLastError() const
{
    return lastErrorCode;
}

std::string to_string(GeneratorErrorCode e)
{
#pragma warning( push )
#pragma warning( default : 4061)
    switch (e)
    {
    case GenErrorNone:
        return "None";
    case GenErrorInvalidJson:
        return "Invalid Json";
    case GenErrorInvalidType:
        return "Invalid Type";
    default:
        break;
    }
#pragma warning ( pop )

    assert(false); // should never get here, but casting and streaming can result in invalid enums
}

//Public Functions
CodeGenerator::CodeGenerator(const std::string indent, const std::string className) : indent(indent), className(className)
{
    this->usingStrings = false;
    this->usingVectors = false;
    this->classCount = 0;
    this->stringHash = std::hash<std::string>();
    this->structureList = std::vector<ObjectData>();
    this->hashSet = std::unordered_map<size_t, size_t>();
    this->lastErrorCode = GenErrorNone;
}

/// <summary>
/// Convert JSON string into a string of C++ classe declarations 
/// </summary>
std::string CodeGenerator::convertJson(std::string& json, const LangFormat& format) {
    this->format = format;
    usingStrings = false;
    usingVectors = false;
    classCount = 0;
    structureList.clear();
    hashSet.clear();
    lastErrorCode = GenErrorNone;

    //Construct JSON document and parse
    rapidjson::Document doc;
    doc.Parse(json.c_str());
    if (doc.HasParseError()) {
        lastErrorCode = GenErrorInvalidJson;
        fmt::print("ERROR: (offset {}}): {}\n",
            (unsigned)doc.GetErrorOffset(),
            GetParseError_En(doc.GetParseError()));
        return "";
    }

    //Get JsonValue* to the root object of the document
    auto* docObj = rapidjson::Pointer("").Get(doc); 
    int depth = 0;
    DeserializeJsonObject(docObj, depth);
    return GenerateCode();
}

/// Private Functions

std::string CodeGenerator::getType(rapidjson::Value* jsonValue, int depth) {
    if (depth > MAX_DEPTH) {
        return format.placeholder_t;
    }

    if (jsonValue->IsArray()) {
        usingVectors = true;
        if (jsonValue->Size() > 0) {
            std::string type = format.placeholder_t;
            for (size_t i = 0; i < jsonValue->Size(); i++)
            {
                std::string tempType = getType(&jsonValue->GetArray()[i], depth + 1);
                if (type == format.placeholder_t && tempType != format.placeholder_t) {
                    type = tempType;
                }
            }
            return type;
        }
        return format.placeholder_t;
    }
    else if (jsonValue->IsObject()) {
        return DeserializeJsonObject(jsonValue, depth + 1);
    }
    else if (jsonValue->IsInt()) {
        return format.int_t;
    }
    else if (jsonValue->IsFloat()) {
        return format.float_t;
    }
    else if (jsonValue->IsUint()) {
        return format.uint_t;
    }
    else if (jsonValue->IsInt64()) {
        return format.ll_t;
    }
    else if (jsonValue->IsUint64()) {
        return format.ull_t;
    }
    else if (jsonValue->IsDouble()) {
        return format.double_t;
    }
    else if (jsonValue->IsString()) {
        usingStrings = true;
        return format.string_t;
    }
    else if (jsonValue->IsBool()) {
        return format.bool_t;
    }
    lastErrorCode = GenErrorInvalidType;
    return typeError;
}

/// <summary>
/// Creates a new ObjectData by recursively searching the jsonValue's members and
/// adds it to the structure list
/// </summary>
/// <param name="jsonValue">Starting node convert into an ObjectData</param>
/// <param name="depth">Current recurse depth</param>
/// <returns>Name of the created or an identical ObjectData</returns>
std::string CodeGenerator::DeserializeJsonObject(rapidjson::Value* jsonValue, int depth) {
    if (depth > MAX_DEPTH) {
        return format.placeholder_t;
    }

    ObjectData sstruct;
    for (auto& member : jsonValue->GetObject())
    {
        std::string typeString = getType(&member.value, depth + 1);

        if (typeString == format.placeholder_t) {
            sstruct.isComplete = false;
        }

        sstruct.members.emplace_back(typeString, member.name.GetString(), member.value.IsArray());

        //IMPORTANT: Do not mix std::string and const char* when formatting... it took 2 hours to find this
        //std::string memberText = fmt::format(format.var_format, typeString.c_str(), member.name.GetString());
    }

    std::string hashValue;
    for (const auto& i : sstruct.members) {
        hashValue += i.name;
    }
    size_t hash = stringHash(hashValue);

    auto iter = hashSet.find(hash);
    if (iter == hashSet.end()) {
        //ObjectData hash does not exist. Increment class counter and add new hash
        sstruct.name = className + std::to_string(++classCount);

        structureList.push_back(std::move(sstruct));
        hashSet.insert({ hash, structureList.size() - 1});

        return structureList[structureList.size() - 1].name;
    }
    else {
        //Comapare variable types to merge if any are still "placeholders"
        ObjectData& org = structureList[iter->second];
        assert(org.members.size() == sstruct.members.size()); //If this assert falls through somthing is wrong :p

        for (size_t i = 0; i < org.members.size(); i++)
        {
            assert(org.members[i].name == sstruct.members[i].name);

            if (org.members[i].type == format.placeholder_t && sstruct.members[i].type != format.placeholder_t) {
                org.members[i].type = sstruct.members[i].type;
                org.members[i].isContainer = sstruct.members[i].isContainer;
            }
        }

        return structureList[iter->second].name;
    }
}

std::string CodeGenerator::GenerateCode() {
    std::string text;
    text += usingVectors ? format.using_vector : "";
    text += usingStrings ? format.using_string : "";
    text += usingStrings || usingVectors ? "\n" : "";

    for (auto& sstruct : structureList)
    {
        text += fmt::format(format.structS_format, sstruct.name);
        for (auto& member : sstruct.members)
        {
            if (member.isContainer) {
                text += indent + fmt::format(format.var_format, fmt::format(format.array_format, member.type), member.name) + "\n";
            }
            else {
                text += indent + fmt::format(format.var_format, member.type, member.name) + "\n";
            }
        }
        text += format.structE_format;
    }

    return text;
}
